AMAZON FREERTOS SDK EXAMPLE

Description:
Start to run Amazon FreeRTOS SDK on Ameba

Configuration:
1. Enable example in component/common/example/example_entry.c. uncomment below 2 line. 
	//#include <amazon_freertos/example_amazon_freertos.h>
	//example_amazon_freertos();


2.  Clone the amebaZ2 amebaZ2-7.1d-202107.00-LTS repository from our Github (https://github.com/Ameba-AIoT/ameba-amazon-freertos/tree/amebaZ2-7.1d-202107.00-LTS)

    Run the following command under the path: SDK/component/common/application/amazon

    $ git clone --recursive --branch amebaZ2-7.1d-202107.00-LTS --depth 1 https://github.com/Ameba-AIoT/ameba-amazon-freertos.git freertos_LTS
	
    then you will see all the source code in file: SDK/component/common/application/amazon/freertos_LTS/

	Work with https://github.com/Ameba-AIoT/ameba-amazon-freertos/tree/amebaZ2-7.1d-202107.00-LTS @ a94ee6fa1902ad98b0d13cdfe798b591a9f35a17



3. Clone the amebaZ2 FreeRTOS-Kernel repository from Github (https://github.com/FreeRTOS/FreeRTOS-Kernel.git)

	Run the following command under the path: SDK/component/os/freertos
	
	git clone --branch V10.4.3-LTS-Patch-2 --depth 1 https://github.com/FreeRTOS/FreeRTOS-Kernel.git freertos_v10.4.3
	
	then you will see all the source code in file: SDK/component/os/freertos/freertos_v10.4.3


   
4.  Configure aws_clientcredential.h and aws_clientcredential_keys.h
	Refer to https://docs.aws.amazon.com/freertos/latest/userguide/freertos-configure.html, which will have the instructions. 

	-In aws_clientcredential.h(SDK/component/common/application/amazon/amazon-freertos/demos/include), set endpoint and wifi info.

		#define clientcredentialMQTT_BROKER_ENDPOINT	"xxxxxxxxxxxxxx.amazonaws.com"
		
		#define clientcredentialIOT_THING_NAME          "xxxxx"

		#define clientcredentialWIFI_SSID				"SSID"
		#define clientcredentialWIFI_PASSWORD			"PASSWORD"

	-In aws_clientcredential_keys.h(SDK/component/common/application/amazon/amazon-freertos/demos/include),set MQTT Demo required credentials
			
		#define keyCLIENT_CERTIFICATE_PEM 			"CERTIFICATE"
		#define keyCLIENT_PRIVATE_KEY_PEM			"PRIVATE_KEY"



5.  For running Amazon SDK example, the following configuration is set.
	
	- In aws_demo_config.h(SDK/component/common/application/amazon/amazon-freertos/vendors/realtek/boards/amebaZ2/aws_demos/config_files/)
	
	//#define CONFIG_CORE_HTTP_MUTUAL_AUTH_DEMO_ENABLED
	#define CONFIG_CORE_MQTT_MUTUAL_AUTH_DEMO_ENABLED
	//#define CONFIG_DEVICE_SHADOW_DEMO_ENABLED
	//#define CONFIG_DEVICE_DEFENDER_DEMO_ENABLED
	//#define CONFIG_OTA_MQTT_UPDATE_DEMO_ENABLED
	//#define CONFIG_OTA_HTTP_UPDATE_DEMO_ENABLED
	//#define CONFIG_JOBS_DEMO_ENABLED
    
	- Default will run CONFIG_CORE_MQTT_MUTUAL_AUTH_DEMO_ENABLED example

6.  For Ameba Z2 project/realtek_amebaz2_v0_example/GCC-RELEASE/

			
		For IgnoreSecure project (GCC):
		
			Configurations are the same. Need to rename "Makefile.amazon" to "Makefile", and replace it with the original "Makefile".

		To make image
			cd to project/realtek_amebaz2_v0_example/GCC-RELEASE
			use command "make clean","make amazon" and "make is" to build image


7. Configure ota_demo_config.h for OTA demo
	-In ota_demo_config.h(SDK/component/common/application/amazon/amazon-freertos/vendors/realtek/boards/amebaZ2/aws_demos/config_files/)
		#define otapalconfigCODE_SIGNING_CERTIFICATE \
		"-----BEGIN CERTIFICATE-----\n" \
		"................................................................\n" \
		"................................................................\n" \
		"................................................................\n" \
		"................................................................\n" \
		"................................................................\n" \
		"................................................................\n" \
		"................................................................\n" \
		".........................\n" \
		"-----END CERTIFICATE-----"



8.  OTA update prerequisites (https://docs.aws.amazon.com/freertos/latest/userguide/ota-prereqs.html)
	Step1. Prerequisites for OTA updates using MQTT
	Step2. Create an Amazon S3 bucket to store your update
	Step3. Create an OTA Update service role
	Step4. Create an OTA user policy
	---
	Step5. Create esdsasigner.key and ecdsasigner.crt by openSSL (optional)
		EX:	sudo openssl ecparam -name prime256v1 -genkey -out ecdsa-sha256-signer.key.pem
			sudo openssl req -new -x509 -days 3650 -key ecdsa-sha256-signer.key.pem -out ecdsa-sha256-signer.crt.pem



9.  How is the custom code signing binary created:
	a.	We are using custom signing feature provided by amazon to manually sign the OTA binary(firmware_is_pad.bin).
	b.	The custom signing is being done by means of a python script that we provid in the folder:  SDK/component/common/example/amazon_freertos/amazon_ota_tools/python_custom_ecdsa_Z2_iar.py
																									SDK/component/common/example/amazon_freertos/amazon_ota_tools/python_custom_ecdsa_Z2_gcc.py
		i.	The python script requires the following pre-requisites to work
			1.	Python must be installed in the windows system with version 3.7.x or later
			2.	Pyopenssl library must be installed using 'pip install pyopenssl'
			3.	The ECDSA signing key and the Certificate pair must be present in the same folder as the python script and must be named 'ecdsa-sha256-signer.key.pem' and 'ecdsa-sha256-signer.crt.pem' respectively.
			
			!!!!!!! The key pair in SDK are just for example, please generated new key by openssl !!!!!!
			!!!!!!! The key pair in SDK are just for example, please generated new key by openssl !!!!!!
			!!!!!!! The key pair in SDK are just for example, please generated new key by openssl !!!!!!
			
			4.	Once all these are present and the python script is run, it will generate a custom signed binary named firmware_is_pad.bin inside the SDK/component/common/example/amazon_freertos/amazon_ota_tools/ folder.
                Run the python script in folder: SDK/component/common/example/amazon_freertos/amazon_ota_tools/
                *shell command after GCC build -> $ python python_custom_ecdsa_Z2_gcc.py

                There might be some error if there are packages lack in your environment (like openssl...). Please install the package and run the script again.

	c.	After getting the custom signed 'firmware_is_pad.bin' and 'IDT-OTA-Signature', you can upload it to the S3 bucket.



10.  How to trigger a custom signed OTA job in amazon AWS IOT core. 
	a.	Click on 'Create OTA update job', select your thing/things group and then select next.
	b.	In the following page, choose the option 'Use my custom signed firmware image'
	c.	Choose your custom signed firmware binary that was generated by the python script from S3 bucket.
	d.	In the signature field, enter signature 'xxxxxxxxxxxxxxx' in 'IDT-OTA-Signature'
	e.	Choose hash algorithm as 'SHA-256'
	f.	Choose encryption algorithm as 'ECDSA'.
	g.	In "pathname of code signing certificate" and the "Pathname of file on device", both enter '/'
	h.	Choose the IAM role for OTA update job.(This is the same IAM role as any OTA update job)
	i.	Click next, give a unique name for the job and create.



