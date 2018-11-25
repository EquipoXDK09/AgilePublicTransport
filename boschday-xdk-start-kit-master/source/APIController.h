#ifndef SOURCE_APICONTROLLER_H_
#define SOURCE_APICONTROLLER_H_

#include "XdkAppInfo.h"
#include <math.h>

/* additional interface header files */
#include "XDK_WLAN.h"
#include "XDK_HTTPRestClient.h"
#include "BCDS_NetworkConfig.h"
char *CAMION_ID_RST= "0000";

uint8_t idCamion[4];

/* WLAN configurations ****************************************************** */
/**
 * WLAN_SSID is the WIFI network name where user wants connect the XDK device.
 * Make sure to update the WLAN_PSK constant according to your required WIFI network.
 */
#define WLAN_SSID                           "Fam. Mora V. GDL"

/**
 * WLAN_PSK is the WIFI router WPA/WPA2 password used at the Wifi network connection.
 * Make sure to update the WLAN_PSK constant according to your router password.
 */
#define WLAN_PSK                            "Luismora3310723050"

/**
 * WLAN_STATIC_IP is a boolean. If "true" then static IP will be assigned and if "false" then DHCP is used.
 */
#define WLAN_STATIC_IP                      false

/**
 * WLAN_IP_ADDR is the WIFI router WPA/WPA2 static IPv4 IP address (unused if WLAN_STATIC_IP is false)
 * Make sure to update the WLAN_IP_ADDR constant according to your required WIFI network,
 * if WLAN_STATIC_IP is "true".
 */
#define WLAN_IP_ADDR                        XDK_NETWORK_IPV4(0, 0, 0, 0)

/**
 * WLAN_GW_ADDR is the WIFI router WPA/WPA2 static IPv4 gateway address (unused if WLAN_STATIC_IP is false)
 * Make sure to update the WLAN_GW_ADDR constant according to your required WIFI network,
 * if WLAN_STATIC_IP is "true".
 */
#define WLAN_GW_ADDR                        XDK_NETWORK_IPV4(0, 0, 0, 0)

/**
 * WLAN_DNS_ADDR is the WIFI router WPA/WPA2 static IPv4 DNS address (unused if WLAN_STATIC_IP is false)
 * Make sure to update the WLAN_DNS_ADDR constant according to your required WIFI network,
 * if WLAN_STATIC_IP is "true".
 */
#define WLAN_DNS_ADDR                       XDK_NETWORK_IPV4(0, 0, 0, 0)

/**
 * WLAN_MASK is the WIFI router WPA/WPA2 static IPv4 mask address (unused if WLAN_STATIC_IP is false)
 * Make sure to update the WLAN_MASK constant according to your required WIFI network,
 * if WLAN_STATIC_IP is "true".
 */
#define WLAN_MASK                           XDK_NETWORK_IPV4(0, 0, 0, 0)

/* Server configurations ***************************************************** */

/**
 * DEST_SERVER_HOST is the host name of the web server we will send HTTP requests to.
 * If you want to test this example without setting up your own server, you can use publicly available services.
 */
#define DEST_SERVER_HOST                 "api.thingspeak.com"

/**
 * DEST_SERVER_PORT is the TCP port to which we will send HTTP requests to.
 * The default of 80 should be fine for most applications.
 */
#define DEST_SERVER_PORT                UINT16_C(80)

/**
 * DEST_SERVER_PORT_SECURE is the TCP port to which we will send HTTPS requests to.
 * The default of 443 should be fine for most applications.
 */
#define DEST_SERVER_PORT_SECURE                UINT16_C(443)

/**
 * DEST_POST_PATH is the path relative to the DEST_SERVER_HOST that we will send
 * the HTTP POST request to.
 *
 * Change this value if you use your own web server.
 */
#define DEST_POST_PATH                  "/update.json"

/**
 * POST_REQUEST_CUSTOM_HEADER_0 is a custom header which is sent along with the
 * POST request. It's meant to demonstrate how to use custom header.
 */
#define POST_REQUEST_CUSTOM_HEADER_0    "Content-Type: application/json\r\n"

/**
 * The time we wait (in milliseconds) between sending HTTP requests.
 */
#define INTER_REQUEST_INTERVAL          UINT32_C(1000)

/**
 * HTTP_SECURE_ENABLE is Set to Use HTTP With Security
 */
#define HTTP_SECURE_ENABLE          UINT32_C(0)

/**
 * The maximum amount of data we download in a single request (in bytes). This number is
 * limited by the platform abstraction layer implementation that ships with the
 * XDK. The maximum value that will work here is 512 bytes.
 */
#define REQUEST_MAX_DOWNLOAD_SIZE       UINT32_C(512)

#define APP_RESPONSE_FROM_HTTP_SERVER_POST_TIMEOUT      UINT32_C(10000)/**< Timeout for completion of HTTP rest client POST */

/* local variables ********************************************************** */

static WLAN_Setup_T WLANSetupInfo =
        {
                .IsEnterprise = false,
                .IsHostPgmEnabled = false,
                .SSID = WLAN_SSID,
                .Username = WLAN_PSK, /* Unused for Personal WPA2 connection */
                .Password = WLAN_PSK,
                .IsStatic = WLAN_STATIC_IP,
                .IpAddr = WLAN_IP_ADDR,
                .GwAddr = WLAN_GW_ADDR,
                .DnsAddr = WLAN_DNS_ADDR,
                .Mask = WLAN_MASK,
        };/**< WLAN setup parameters */

static HTTPRestClient_Setup_T HTTPRestClientSetupInfo =
        {
                .IsSecure = HTTP_SECURE_ENABLE,
        };/**< HTTP rest client setup parameters */

static HTTPRestClient_Config_T HTTPRestClientConfigInfo =
        {
                .IsSecure = HTTP_SECURE_ENABLE,
                .DestinationServerUrl = DEST_SERVER_HOST,
                .DestinationServerPort = DEST_SERVER_PORT,
                .RequestMaxDownloadSize = REQUEST_MAX_DOWNLOAD_SIZE,
        }; /**< HTTP rest client configuration parameters */


/* functions ********************************************************** */

static void AppControllerValidateWLANConnectivity(void)
{
    Retcode_T retcode = RETCODE_OK;
    NetworkConfig_IpStatus_T ipStatus = NETWORKCONFIG_IP_NOT_ACQUIRED;
    NetworkConfig_IpSettings_T ipAddressOnGetStatus;

    ipStatus = NetworkConfig_GetIpStatus();
    if (ipStatus == NETWORKCONFIG_IPV4_ACQUIRED)
    {
        retcode = NetworkConfig_GetIpSettings(&ipAddressOnGetStatus);
        if ((RETCODE_OK == retcode) && (UINT32_C(0) == (ipAddressOnGetStatus.ipV4)))
        {
            /* Our IP configuration is corrupted somehow in this case. No use in proceeding further. */
            retcode = RETCODE(RETCODE_SEVERITY_ERROR, RETCODE_NODE_IPV4_IS_CORRUPTED);
        }
    }
    else
    {
        /* Our network connection is lost. No use in proceeding further. */
        retcode = RETCODE(RETCODE_SEVERITY_ERROR, RETCODE_NODE_WLAN_CONNECTION_IS_LOST);
    }
    if (RETCODE_OK != retcode)
    {
        Retcode_RaiseError(retcode);
        printf("AppControllerValidateWLANConnectivity : Resetting the device. Check if network is available. Node will do a soft reset in 10 seconds.\r\n\r\n");
        vTaskDelay(pdMS_TO_TICKS(10000));
        BSP_Board_SoftReset();
        assert(false); /* Code must not reach here */
    }
}


static void sendAPIData(Sensor_Value_T * sensorValue)
{
	int velocidad=0;
	char jsonFile[1024];
	int sizeJson = sprintf (jsonFile, "{ \"api_key\": \"ZTFZ0WIHLOAOTLIQ\", \"field1\": \"%ld %ld %ld\", \"field5\": \"%f\" , \"field6\": \"%s\" }"
			, sensorValue->Accel.X, sensorValue->Accel.Y, sensorValue->Accel.Z,sensorValue->Temp,idCamion);

	if(strcmp((const char *)idCamion,"0000")!= 0UL){
		sprintf(idCamion,"0000");
	}

	HTTPRestClient_Post_T HTTPRestClientPostInfo =
	        {
	                .Payload = jsonFile,
	                .PayloadLength = (sizeJson),
	                .Url = DEST_POST_PATH,
	                .RequestCustomHeader0 = POST_REQUEST_CUSTOM_HEADER_0,
	        }; /**< HTTP rest client POST parameters */

    Retcode_T retcode = RETCODE_OK;

        /* Resetting / clearing the necessary buffers / variables for re-use */
        retcode = RETCODE_OK;

        /* Check whether the WLAN network connection is available */
        AppControllerValidateWLANConnectivity();

        /* Do a HTTP rest client POST */
        retcode = HTTPRestClient_Post(&HTTPRestClientConfigInfo, &HTTPRestClientPostInfo, APP_RESPONSE_FROM_HTTP_SERVER_POST_TIMEOUT);
        if (RETCODE_OK == retcode)
        {
            /* Wait for INTER_REQUEST_INTERVAL */
            vTaskDelay(pdMS_TO_TICKS(INTER_REQUEST_INTERVAL));
            }
        if (RETCODE_OK == retcode)
        {
            /* Wait for INTER_REQUEST_INTERVAL */
            vTaskDelay(pdMS_TO_TICKS(INTER_REQUEST_INTERVAL));
        }
        if (RETCODE_OK != retcode)
        {
            /* Report error and continue */
            Retcode_RaiseError(retcode);
        }

}

#endif /* SOURCE_APICONTROLLER_H_ */
