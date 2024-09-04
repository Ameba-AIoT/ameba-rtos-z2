#include "platform_opts.h"

#if defined(CONFIG_EXAMPLE_MBEDTLS_ECDHE) && (CONFIG_EXAMPLE_MBEDTLS_ECDHE == 1)
#include "FreeRTOS.h"
#include "task.h"
#include <platform/platform_stdlib.h>
#include "diag.h"

#include <mbedtls/ssl.h>
#include <mbedtls/ecdh.h>
#include <mbedtls/entropy.h>
#include <mbedtls/ctr_drbg.h>
#include <mbedtls/platform.h>
#if (CONFIG_MBEDTLS_VERSION3 != 1)
#include "mbedtls/ssl_internal.h"
#endif
#define BUFSIZE         1024
#define  mbedtls_printf printf
#define time_t long long

static void *my_calloc(size_t nelements, size_t elementSize)
{
	size_t size;
	void *ptr = NULL;

	size = nelements * elementSize;
	ptr = pvPortMalloc(size);

	if (ptr) {
		memset(ptr, 0, size);
	}

	return ptr;
}

static void example_mbedtls_ecdhe_thread(void *param)
{

	/* To avoid gcc warnings */
	(void) param;

	int ret;
	mbedtls_ecdh_context ctx_cli, ctx_srv;
	mbedtls_entropy_context entropy;
	mbedtls_ctr_drbg_context ctr_drbg;
	unsigned char cli_to_srv[32], srv_to_cli[32];
	const char pers[] = "ecdh";

	mbedtls_ecdh_init(&ctx_cli);
	mbedtls_ecdh_init(&ctx_srv);
	mbedtls_ctr_drbg_init(&ctr_drbg);
	mbedtls_platform_set_calloc_free(my_calloc, vPortFree);

	/*
	 * Initialize random number generation
	 */
	mbedtls_printf("  . Seeding the random number generator...");
	mbedtls_entropy_init(&entropy);
	if ((ret = mbedtls_ctr_drbg_seed(&ctr_drbg, mbedtls_entropy_func, &entropy,
									 (const unsigned char *) pers,
									 sizeof pers)) != 0) {
		printf("\n\r%s: line %d", __func__, __LINE__);
		mbedtls_printf(" failed\n	! mbedtls_ctr_drbg_seed returned %d\n", ret);
		goto exit;
	}

	mbedtls_printf(" ok\n");

	/*
	 * Client: inialize context and generate keypair
	 */
	mbedtls_printf("  . Setting up client context...");

	ret = mbedtls_ecp_group_load(&ctx_cli.grp, MBEDTLS_ECP_DP_CURVE25519);
	if (ret != 0) {
		mbedtls_printf(" failed\n	! mbedtls_ecp_group_load returned %d\n", ret);
		goto exit;
	}
	ret = mbedtls_ecdh_gen_public(&ctx_cli.grp, &ctx_cli.d, &ctx_cli.Q,
								  mbedtls_ctr_drbg_random, &ctr_drbg);
	if (ret != 0) {
		mbedtls_printf(" failed\n	! mbedtls_ecdh_gen_public returned %d\n", ret);
		goto exit;
	}

	ret = mbedtls_mpi_write_binary(&ctx_cli.Q.X, cli_to_srv, 32);
	if (ret != 0) {
		mbedtls_printf(" failed\n	! mbedtls_mpi_write_binary returned %d\n", ret);
		goto exit;
	}

	mbedtls_printf(" ok\n");

	/*
	 * Server: initialize context and generate keypair
	 */
	mbedtls_printf("  . Setting up server context...");

	ret = mbedtls_ecp_group_load(&ctx_srv.grp, MBEDTLS_ECP_DP_CURVE25519);
	if (ret != 0) {
		mbedtls_printf(" failed\n	! mbedtls_ecp_group_load returned %d\n", ret);
		goto exit;
	}

	ret = mbedtls_ecdh_gen_public(&ctx_srv.grp, &ctx_srv.d, &ctx_srv.Q,
								  mbedtls_ctr_drbg_random, &ctr_drbg);
	if (ret != 0) {
		mbedtls_printf(" failed\n	! mbedtls_ecdh_gen_public returned %d\n", ret);
		goto exit;
	}

	ret = mbedtls_mpi_write_binary(&ctx_srv.Q.X, srv_to_cli, 32);
	if (ret != 0) {
		mbedtls_printf(" failed\n	! mbedtls_mpi_write_binary returned %d\n", ret);
		goto exit;
	}

	mbedtls_printf(" ok\n");

	/*
	 * Server: read peer's key and generate shared secret
	 */
	mbedtls_printf("  . Server reading client key and computing secret...");

	ret = mbedtls_mpi_lset(&ctx_srv.Qp.Z, 1);
	if (ret != 0) {
		mbedtls_printf(" failed\n	! mbedtls_mpi_lset returned %d\n", ret);
		goto exit;
	}

	ret = mbedtls_mpi_read_binary(&ctx_srv.Qp.X, cli_to_srv, 32);
	if (ret != 0) {
		mbedtls_printf(" failed\n	! mbedtls_mpi_read_binary returned %d\n", ret);
		goto exit;
	}

	ret = mbedtls_ecdh_compute_shared(&ctx_srv.grp, &ctx_srv.z,
									  &ctx_srv.Qp, &ctx_srv.d,
									  mbedtls_ctr_drbg_random, &ctr_drbg);
	if (ret != 0) {
		mbedtls_printf(" failed\n	! mbedtls_ecdh_compute_shared returned %d\n", ret);
		goto exit;
	}

	mbedtls_printf(" ok\n");

	/*
	 * Client: read peer's key and generate shared secret
	 */
	mbedtls_printf("  . Client reading server key and computing secret...");

	ret = mbedtls_mpi_lset(&ctx_cli.Qp.Z, 1);
	if (ret != 0) {
		mbedtls_printf(" failed\n	! mbedtls_mpi_lset returned %d\n", ret);
		goto exit;
	}

	ret = mbedtls_mpi_read_binary(&ctx_cli.Qp.X, srv_to_cli, 32);
	if (ret != 0) {
		mbedtls_printf(" failed\n	! mbedtls_mpi_read_binary returned %d\n", ret);
		goto exit;
	}

	ret = mbedtls_ecdh_compute_shared(&ctx_cli.grp, &ctx_cli.z,
									  &ctx_cli.Qp, &ctx_cli.d,
									  mbedtls_ctr_drbg_random, &ctr_drbg);
	if (ret != 0) {
		mbedtls_printf(" failed\n	! mbedtls_ecdh_compute_shared returned %d\n", ret);
		goto exit;
	}

	mbedtls_printf(" ok\n");

	/*
	 * Verification: are the computed secret equal?
	 */
	mbedtls_printf("  . Checking if both computed secrets are equal...");

	ret = mbedtls_mpi_cmp_mpi(&ctx_cli.z, &ctx_srv.z);
	if (ret != 0) {
		mbedtls_printf(" failed\n	! mbedtls_ecdh_compute_shared returned %d\n", ret);
		goto exit;
	}

	mbedtls_printf(" ok\n");

exit:

#if defined(_WIN32)
	mbedtls_printf("  + Press Enter to exit this program.\n");
	fflush(stdout);
	getchar();
#endif

	mbedtls_ecdh_free(&ctx_srv);
	mbedtls_ecdh_free(&ctx_cli);
	mbedtls_ctr_drbg_free(&ctr_drbg);
	mbedtls_entropy_free(&entropy);

	vTaskDelete(NULL);

	return;
}


void example_mbedtls_ecdhe(void)
{
	if (xTaskCreate(example_mbedtls_ecdhe_thread, ((const char *)"example_mbedtls_ecdhe_thread"), 2048, NULL, tskIDLE_PRIORITY + 1, NULL) != pdPASS) {
		printf("\n\r%s xTaskCreate(init_thread) failed", __FUNCTION__);
	}
}

#endif //#if defined(CONFIG_EXAMPLE_MBEDTLS_ECDHE) && (CONFIG_EXAMPLE_MBEDTLS_ECDHE == 1)