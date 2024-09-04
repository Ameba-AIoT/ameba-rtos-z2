#include "platform_opts.h"

#if defined(CONFIG_EXAMPLE_EAP) && CONFIG_EXAMPLE_EAP
#include "platform/platform_stdlib.h"
#include "FreeRTOS.h"
#include "task.h"
#include "wifi_conf.h"
#include <eap/example_eap.h>
#if defined(configENABLE_TRUSTZONE) && (configENABLE_TRUSTZONE == 1) && defined(CONFIG_SSL_CLIENT_PRIVATE_IN_TZ) && (CONFIG_SSL_CLIENT_PRIVATE_IN_TZ == 1)
#include "device_lock.h"
#endif

#define STACKSIZE	1024

#if !defined(CONFIG_MBED_ENABLED)
extern int eap_start(char *method);
#endif

// get config arguments from wifi_eap_config.c
extern char *eap_target_ssid;
extern char *eap_identity;
extern char *eap_password;
extern const unsigned char *eap_ca_cert;
extern const unsigned char *eap_client_cert;
extern const unsigned char *eap_client_key;
extern int eap_ca_cert_len;
extern int eap_client_cert_len;
extern int eap_client_key_len;
extern char *eap_client_key_pwd;

void example_eap_config(void)
{
	eap_target_ssid = "Test_eap";
	eap_identity = "guest2";
	eap_password = "test2";

#if CONFIG_ENABLE_TLS
	/*
		Set eap_client_cert and eap_client_key only for EAP-TLS connection.
	*/
	eap_client_cert = \
					  "-----BEGIN CERTIFICATE-----\r\n" \
					  "MIIC9TCCAd0CAQIwDQYJKoZIhvcNAQEEBQAwgZMxCzAJBgNVBAYTAkZSMQ8wDQYD\r\n" \
					  "VQQIEwZSYWRpdXMxEjAQBgNVBAcTCVNvbWV3aGVyZTEVMBMGA1UEChMMRXhhbXBs\r\n" \
					  "ZSBJbmMuMSAwHgYJKoZIhvcNAQkBFhFhZG1pbkBleGFtcGxlLmNvbTEmMCQGA1UE\r\n" \
					  "AxMdRXhhbXBsZSBDZXJ0aWZpY2F0ZSBBdXRob3JpdHkwHhcNMTYwMzE0MTEzNjMy\r\n" \
					  "WhcNMTcwMzE0MTEzNjMyWjBxMQswCQYDVQQGEwJGUjEPMA0GA1UECBMGUmFkaXVz\r\n" \
					  "MRUwEwYDVQQKEwxFeGFtcGxlIEluYy4xGTAXBgNVBAMUEHVzZXJAZXhhbXBsZS5j\r\n" \
					  "b20xHzAdBgkqhkiG9w0BCQEWEHVzZXJAZXhhbXBsZS5jb20wgZ8wDQYJKoZIhvcN\r\n" \
					  "AQEBBQADgY0AMIGJAoGBAODvCWRRjVQnUyQS/OqHS8MA94Dc5UOtLagKTOMJayB5\r\n" \
					  "3MZyreWBkNg6sDfDG6OSD9tkVzwcp8CtZNflJc3i+d+nAnPM+kJedPJN5YVO+uwc\r\n" \
					  "+K+QObH7fEOq8hnFIvOtYOfnMAxQKaVIKk0EOqqQv06BDvLyxoDCZNpAn4NQ8ZkR\r\n" \
					  "AgMBAAEwDQYJKoZIhvcNAQEEBQADggEBAItqpmFftRu8ugTy4fRFwpjJNUuMRe83\r\n" \
					  "Pm5Dv3V/byCHHdmIy0UI+6ZiMEtYrpvz4ZPgk0BDeytYooT7/kEUb8niQ64bDLYo\r\n" \
					  "NcXctCmn5fjyX2M6Z3lQXCxX0XdFiukWlR21w4HO0nx7OJjrcjdpP9Tyk/kzCFl7\r\n" \
					  "pblIavkfSmFtcxzcp0IoCupkUjFkA+MftZF82eQx4bE0jjiw2KgGwnzyYAdgtFXv\r\n" \
					  "Ednj3ZyOuTlOQNGJgLQxyHooEJ/Tol/8p9EO5S6eQaHgZhbGP3AZ3SWV5oA0e6eT\r\n" \
					  "D5JXti/LhyZhcbbJFawGXFI96ZOpHJ0EW12Osx/21oqmMp12AotS5Vw=\r\n" \
					  "-----END CERTIFICATE-----\r\n";

	eap_client_cert_len = strlen(eap_client_cert) + 1;

#if defined(configENABLE_TRUSTZONE) && (configENABLE_TRUSTZONE == 1) && defined(CONFIG_SSL_CLIENT_PRIVATE_IN_TZ) && (CONFIG_SSL_CLIENT_PRIVATE_IN_TZ == 1)
//client key is moved to secure zone, please config "client_key_s" in main_s.c.
#else
	/*
		PKCS#8 key format is not supported by mbedtls, please check readme for solution.
	*/
	/*
		Example 1: rsa format encrypt key
		encrypt key format need to set eap_client_key_pwd for decryption, otherwise leave them to NULL

		eap_client_key = \
					"-----BEGIN RSA PRIVATE KEY-----\r\n" \
					"Proc-Type: 4,ENCRYPTED\r\n" \
					"DEK-Info: DES-EDE3-CBC,79675299AD6E2237\r\n" \
					"\r\n" \
					"ZYY2hv1PYEsrhYbCip98XNpS6XxbntynEEp6aO9UgWeQ4I1pNOUptPUE+yNhbA7X\r\n" \
					"59ueT3yzx5L2ObImlJ3eIEvWq+iB8DdcPqFAo3c4dgfw/wPEhmxVPKvIyDQfaEuA\r\n" \
					"kWUno6b07n5uLTpQjIXQSdMTMYjYS+yPQy7ONC/vl/Ce+RMzrQAZkp5xcNNarUpl\r\n" \
					"2J1D2t+eRih/zRrgeVXztMiW2uyIT5a0IPoeBTPkPVb00kWYzn8eT9doN/ZCyr83\r\n" \
					"mv/uXF5ZOHnSNleOn1NiCZ8Uu3SHnmGhMBBMI75OghpEezQQCmtefYvtRxzGjMVB\r\n" \
					"UoRIlbATAleUjk3bmqRxfA2QZJj/GFWc9grxEerHWrdThSQ0w+fvwKBjTmEtUO2+\r\n" \
					"stKBJQi9RKFq4naM8UhtxojHIscXCx/wKrRZHS4QJYOQYelzfhTRUuTf3Czm/iTh\r\n" \
					"MQvX7dITNlLE3SW2MjzHb2ON9qUaKVnQPk53DO1zYgoxgDbQrw6FXDNMtYVv8SYf\r\n" \
					"JJZp66jGX6e1t4ziPHVqlDi5D2nWQ2DPNHO/rsoydA7icncKsC0iVzeUm7XgesxD\r\n" \
					"QEZoQIQDVS1aRE7qJCk9S2Hfe5Gfqnrp4110YuN/4khjMW2cOCKa/Yjgjyy2QQXT\r\n" \
					"nn6dBAeSWGzRM059VzhOyls5FIfnJIisZvF3JG518SzBU/YUGHEVN1XsfDS2M9/q\r\n" \
					"VkqhJ8/vbmIddKGeYULYW+xs3LvU1hnWiOodd9tuSeg5PxAbkJsV1nW06mVkgBqA\r\n" \
					"zqqEvwvY+6+9QW4PClKNKSocvM6yC+uhRi0sOZ+ckOv7f+uuMyw5FQ==\r\n" \
					"-----END RSA PRIVATE KEY-----\r\n";
					eap_client_key_pwd = "testca";
	*/
	/*
	Example 2: unencrypted plain key format
	*/
	eap_client_key = \
					 "-----BEGIN PRIVATE KEY-----\r\n" \
					 "MIICXQIBAAKBgQCoRKVXg5h8FwiBn4KzyZamIGOG8qhrT+Qh7uxqqEJ7mriyejHS\r\n" \
					 "N2RSktxzi86ze9DtkaeKapOW0D2gXpc1TNNZbi+UlMoR+sJYrTzmXtwMcW8dq8fc\r\n" \
					 "Tq+4/vWUjuOrg8tdujZO9kr64G6XijOPNVaKFpkG3Px4serjjqH8P+mTOQIDAQAB\r\n" \
					 "AoGARI+LyweshssfxSkIKVc3EcNaqi6PHwJzUrw2ChM624AkR1xwllXJg7ehKVdK\r\n" \
					 "xmjprRLO8CASuL1qjsBb3fTKnBl+sIVxIFS0AI4Y3ri8VUKbangvSsI7pCzAFry7\r\n" \
					 "p1gmy9WWRV2ZEa+dV8xcrjb3bloT7hcdeLehgBCvExJIQM0CQQDXlSAKdW3AhYyj\r\n" \
					 "1A+pfyBSGxJbpSwNyyWgwHIHHjxendxmdUbrc8EbAu1eNKbP58TLgdCZsKcMonAv\r\n" \
					 "MY1Y2/nnAkEAx9CrUaCU8pJqXTRypM5JtexLKnYMJhpnA9uUILBQOq4Oe0eruyF5\r\n" \
					 "SaSxhyJYXY491ahWYPF0PTb3jkUhoN+l3wJBAJZthjgGDJlEFwjSFkOtYz4nib3N\r\n" \
					 "GVpeoFj1MBvrazCScpJDz0LIOLzCZCNSFfwIu3dNk+NKMqZMSn+D0h9pD40CQQC5\r\n" \
					 "K9n4NXaTLbjAU2CC9mE85JPr76XmkcUxwAWQHZTcLH1jJdIyAx1hb+zNLLjzSmRn\r\n" \
					 "Yi9ae6ibKhtUjyBQ87HFAkA2Bb3z7NUx+AA2g2HZocFZFShBxylACyQkl8FAFZtf\r\n" \
					 "osudmKdFQHyAWuBMex4tpz/OLTqJ1ecL1JQeC7OvlpEX\r\n" \
					 "-----END PRIVATE KEY-----\r\n";

	eap_client_key_len = strlen(eap_client_key) + 1;
#endif
#endif

#if ENABLE_EAP_SSL_VERIFY_SERVER
	eap_ca_cert = \
				  "-----BEGIN CERTIFICATE-----\r\n" \
				  "MIIEpzCCA4+gAwIBAgIJAPvZaozpdfjkMA0GCSqGSIb3DQEBCwUAMIGTMQswCQYD\r\n" \
				  "VQQGEwJGUjEPMA0GA1UECBMGUmFkaXVzMRIwEAYDVQQHEwlTb21ld2hlcmUxFTAT\r\n" \
				  "BgNVBAoTDEV4YW1wbGUgSW5jLjEgMB4GCSqGSIb3DQEJARYRYWRtaW5AZXhhbXBs\r\n" \
				  "ZS5jb20xJjAkBgNVBAMTHUV4YW1wbGUgQ2VydGlmaWNhdGUgQXV0aG9yaXR5MB4X\r\n" \
				  "DTE2MDMxNDExMjU0OVoXDTE2MDQxMzExMjU0OVowgZMxCzAJBgNVBAYTAkZSMQ8w\r\n" \
				  "DQYDVQQIEwZSYWRpdXMxEjAQBgNVBAcTCVNvbWV3aGVyZTEVMBMGA1UEChMMRXhh\r\n" \
				  "bXBsZSBJbmMuMSAwHgYJKoZIhvcNAQkBFhFhZG1pbkBleGFtcGxlLmNvbTEmMCQG\r\n" \
				  "A1UEAxMdRXhhbXBsZSBDZXJ0aWZpY2F0ZSBBdXRob3JpdHkwggEiMA0GCSqGSIb3\r\n" \
				  "DQEBAQUAA4IBDwAwggEKAoIBAQC9pireu0aCDLNfMaGv3vId7RXjUhQwSK0jV2Oc\r\n" \
				  "SyvlKWH3P/N+5kLrP2iL6SCzyETVDXZ0vOsAMjcBF0zHp16prXV0d51cTUqeWBb0\r\n" \
				  "I5UnGxleIuuOfSg8zLUJoBWZPqLv++eZ5WgOKHt7SXocjvg7TU5t/TMB0Y8OCz3H\r\n" \
				  "CW2vJ/XKMgMA9HDUu4g57cJu88i1JPRpyFaz/HIQBc7+UNb9z+q09uTZKWTmEMqi\r\n" \
				  "E2U0EEIs7EtbxnOze1/8C4XNlmztrEdwvu6UEBU/TFkUoh9M646NkkBK7wP9n9pv\r\n" \
				  "T0nPQRJiiCrICzVqUtlEi9lIKpbBSMbQ0KzrGF7lGTgm4rz9AgMBAAGjgfswgfgw\r\n" \
				  "HQYDVR0OBBYEFIVyecka74kvOKIW0BjlTc/B+a2NMIHIBgNVHSMEgcAwgb2AFIVy\r\n" \
				  "ecka74kvOKIW0BjlTc/B+a2NoYGZpIGWMIGTMQswCQYDVQQGEwJGUjEPMA0GA1UE\r\n" \
				  "CBMGUmFkaXVzMRIwEAYDVQQHEwlTb21ld2hlcmUxFTATBgNVBAoTDEV4YW1wbGUg\r\n" \
				  "SW5jLjEgMB4GCSqGSIb3DQEJARYRYWRtaW5AZXhhbXBsZS5jb20xJjAkBgNVBAMT\r\n" \
				  "HUV4YW1wbGUgQ2VydGlmaWNhdGUgQXV0aG9yaXR5ggkA+9lqjOl1+OQwDAYDVR0T\r\n" \
				  "BAUwAwEB/zANBgkqhkiG9w0BAQsFAAOCAQEAZYHM26sxbKOckVqJJ1QY0U2QFlGP\r\n" \
				  "1GYd8v27znxdnRmSonDvv3GjFfhwoyDk0JUuxkK/33ikCxihrgoO/EQTY9BV2OpW\r\n" \
				  "qkB1PDtb3i5ZRNvfjmW0pVA4p+GmdTGaEE5pTlcVnorzVrUeFKaZakb+IDFYzmeF\r\n" \
				  "xp8B3Bb5wvinDligLOaJnSlgS8QeeIab9HZfaVTTuPmVK6zE6D54Y0dJPnykvDdE\r\n" \
				  "cGN0FC+migfilFjJgkDJ0r78nwes55L8zjoofiZuO03rrHww6ARc3v1jYzAufddk\r\n" \
				  "QTiZHgjlMQb2XXMmXLn8kBgoDnqkXFNe8j0h8uxIJSrjOoIyn1h1wvX5/w==\r\n" \
				  "-----END CERTIFICATE-----\r\n";

	eap_ca_cert_len = strlen((const char *)eap_ca_cert) + 1;
#endif
}

static void example_eap_thread(void *method)
{
#if CONFIG_ENABLE_TLS && defined(configENABLE_TRUSTZONE) && (configENABLE_TRUSTZONE == 1) && defined(CONFIG_SSL_CLIENT_PRIVATE_IN_TZ) && (CONFIG_SSL_CLIENT_PRIVATE_IN_TZ == 1)
	extern void rtw_create_secure_context(u32 secure_stack_size);
	rtw_create_secure_context(STACKSIZE * 4);
	extern int NS_ENTRY secure_mbedtls_platform_set_calloc_free(void);
	secure_mbedtls_platform_set_calloc_free();
	extern void NS_ENTRY secure_set_ns_device_lock(void (*device_mutex_lock_func)(uint32_t), void (*device_mutex_unlock_func)(uint32_t));
	secure_set_ns_device_lock(device_mutex_lock, device_mutex_unlock);
#endif

	/* Wait for wlan init */
	while (!(wifi_is_up(RTW_STA_INTERFACE))) {
		vTaskDelay(1000);
	}

	example_eap_config();

	if (strcmp(method, "tls") == 0) {
		// tls must present client_cert, client_key
		eap_start("tls");
	} else if (strcmp(method, "peap") == 0) {
		eap_start("peap");
	} else if (strcmp(method, "ttls") == 0) {
		eap_start("ttls");
	} else {
		printf("Invalid method\n");
	}

	vTaskDelete(NULL);
}

void example_eap(char *method)
{
	if (xTaskCreate(example_eap_thread, ((const char *)"example_eap_thread"), STACKSIZE, method, tskIDLE_PRIORITY + 1, NULL) != pdPASS) {
		printf("\n\r%s xTaskCreate failed\n", __FUNCTION__);
	}
}

#endif //#if defined(CONFIG_EXAMPLE_EAP) && CONFIG_EXAMPLE_EAP