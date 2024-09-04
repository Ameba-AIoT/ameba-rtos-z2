Fault Message Redirection

Introductions:

	The default configuration of the fault log was set to output exclusively through the UART port, and this brings limitations as it does not allow saving logs to alternative storage mediums.
	To enable more flexible storage, fault message redirection was introduced. Users can save fault logs to any storage medium.

	The Fault Message Redirection functions are as follow:
		* fault_handler_override() is used to redeclare fault handler to utilize RAM code for fault triggering. It requires two callback functions, fault_log() and bt_log() to handle the logs obtained.
		* fault_log() is the callback functions to handle fault event logs, which includes register and stack memory dump logs.
		* bt_log() is the callback functions to handle stack backtrace logs.
		* read_last_fault_log() is to read the last fault event logs and backtrace logs.
		* fault_handler_set_ns_func() is used to set non-secure function for secure region to utilize.
		* secure_fault_handler_override() is used to redeclare fault handler in the secure region.

Working with `ignore secure` project:

	(1) Add functions in main():

		read_last_fault_log();
		fault_handler_override(fault_log, bt_log);

	(2) Add fault_handler.c into application.is.ewp or application.is.mk

	(3) Declare the flash region in platform_opts.h

		/* 
		 * For fault message redirection
		 * NOTE: this offset is safe on pure SDK, and may conflict with user's layout
		 *       please check layout to solve confliction before using
		*/
		#define FAULT_LOG1				(0x200000 - 0x64000) //Store fault log (Reserved max size: 8K)
		#define FAULT_LOG2				(0x200000 - 0x66000) //Store backtrace log (Reserved max size: 8K)
		#define FAULT_FLASH_SECTOR_SIZE	(6*1024)

Working with `trustzone` project:

	(1) Add functions in main():

		read_last_fault_log();
		fault_handler_override(fault_log, bt_log);
		fault_handler_set_ns_func();
		secure_main();

	(2) Add functions in secure_main() or any secure api:

		secure_fault_handler_override()

	(3) Add fault_handler.c into application.ns.ewp or application.ns.mk

	(4) Add fault_handler_s.c into application.s.ewp or application.s.mk

	(5) Declare the flash region in platform_opts.h

		/* 
		 * For fault message redirection
		 * NOTE: this offset is safe on pure SDK, and may conflict with user's layout
		 *       please check layout to solve confliction before using
		*/
		#define FAULT_LOG1				(0x200000 - 0x64000) //Store fault log (Reserved max size: 8K)
		#define FAULT_LOG2				(0x200000 - 0x66000) //Store backtrace log (Reserved max size: 8K)
		#define FAULT_FLASH_SECTOR_SIZE	(6*1024)

Expected Results:

	When a fault occurs, the log is saved to flash memory via fault_log() and bt_log().
	After the system restarts, the function read_last_fault_log() is called to retrieve the latest fault log from flash memory.

Reference:

	For detailed implementation guidance, users should refer to the files in `example_sources/fault_message_redirection`, which serves as a practical example of how to integrate these logging capabilities into their projects.