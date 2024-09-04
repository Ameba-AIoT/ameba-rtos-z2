OTA HTTPS UPDATING EXAMPLE

Description:
This example is designed for firmware update by Over-the-air programming (OTA) via Wireless Network Connection.
Download firmware from the https download server, like an apache server.


Configuration:
1. Modify MBEDTLS_SSL_MAX_CONTENT_LEN in SSL config if the transmitted fils size is larger than 16kbytes.
[mbedtls/config_rsa.h]
    #define MBEDTLS_SSL_MAX_CONTENT_LEN    16384

2. (if needed) According to the SSL server ciphersuite, enable related config in config_rsa.h to support the ciphersuite.

3. Modify PORT, HOST and RESOURCE based on your HTTPS download server.
[example_ota_https.c]
    e.g.: SERVER: https://m-apps.oss-cn-shenzhen.aliyuncs.com/051103061600.bin
    set:    #define PORT    443
            #define HOST    "m-apps.oss-cn-shenzhen.aliyuncs.com"
            #define RESOURCE    "051103061600.bin"
    For local HTTP server download, Set it with IP and firmware_xx.bin
    set:    #define PORT    443
            #define HOST    "192.168.1.100"
            #define RESOURCE    "firmware_xx.bin"

4. Enable example.
[platform_opts.h]
    #define CONFIG_EXAMPLE_OTA_HTTPS    1
[ota_8710c.h]
    #define HTTPS_OTA_UPDATE


Execution:
Can make automatical Wi-Fi connection when booting by using wlan fast connect example.
A https OTA download example thread will be started automatically when booting.
To verify the TLS sessions, you can setup an apache server for yourself and make sure the KeepAliveTimeout is larger than 5s.


[Supported List]
    Supported :
        Ameba-z2