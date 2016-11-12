/*
 * usrTasks.c
 *
 *  Created on: 14 wrz 2016
 *      Author: Patryk
 */

#include "usrTasks.h"

extern uint8_t DHCP_state;
extern struct netif gnetif;

StmConfig* configStr;
SoundBuffer* mainSoundBuffer;
uint16_t temporaryAudioBuffer[AUDIO_BUFFER_SIZE];
uint32_t audioSamplingFrequency = 44100;
AmplitudeStr* sharedAmplitude;

osThreadId initTaskHandle;
osThreadDef(initThread, initTask, osPriorityNormal, 1,
		configMINIMAL_STACK_SIZE);

#ifdef LCD_PRINTER_SUPPORT
osThreadId lcdTaskHandle;
osThreadDef(lcdThread, lcdTask, osPriorityNormal, 1, configMINIMAL_STACK_SIZE);
#endif

osThreadId samplingTaskHandle;
osThreadDef(samplingThread, samplingTask, osPriorityRealtime, 1,
		configMINIMAL_STACK_SIZE);

osThreadId streamingTaskHandle;
osThreadDef(streamingThread, streamingTask, osPriorityRealtime, 1,
		5*configMINIMAL_STACK_SIZE);

osThreadId httpConfigTaskHandle;
osThreadDef(httpConfigThread, httpConfigTask, osPriorityHigh, 1,
		4*configMINIMAL_STACK_SIZE);

osThreadId dhcpTaskHandle;
osThreadDef(dhcpThread, dhcpTask, osPriorityNormal, 1,
		configMINIMAL_STACK_SIZE*5);

osThreadId soundProcessingTaskHandle;
osThreadDef(soundProcessingThread, soundProcessingTask, osPriorityHigh, 1,
		25000);

osPoolDef(soundBufferPool, 1, SoundBuffer);
osPoolId soundBufferPool_id;
osPoolDef(amplitudePool, 2, AmplitudeStr);
osPoolId amplitudePool_id;
osPoolDef(cfftPool, 1, arm_cfft_instance_f32);
osPoolId cfftPool_id;
osPoolDef(processingBufferPool, 1, float32_t[SOUND_BUFFER_MAX_BUFFER_SIZE]);
osPoolId processingBufferPool_id;
osPoolDef(stmConfigPool, 1, StmConfig);
osPoolId stmConfigPool_id;

osMailQDef(audioMail_q, MAXIMUM_AUDIO_MESSAGE_QUEUE_SIZE, SoundMail);
osMailQId audioMail_q_id;

osMutexDef(sharedAmplitudeMutex);
osMutexId sharedAmplitudeMutex_id;

osMutexDef(ethernetMutex);
osMutexId ethernetMutex_id;

osMutexDef(samplingMutex);
osMutexId samplingMutex_id;

void threadsInit() {
	initTaskHandle = osThreadCreate(osThread(initThread), NULL);
}

void initTask(void const * argument) {
	/* PERIPHERALS INITIALIZATION */
	MX_FMC_Init();
	MX_LTDC_Init();
	MX_DMA2D_Init();
	MX_GPIO_Init();
	lcdInit();
	logMsg("Ethernet initialization...");
	MX_LWIP_Init();

//DHCP INIT TASK
	dhcpTaskHandle = osThreadCreate(osThread(dhcpThread), NULL);

//WAITING FOR DHCP INITIALIZATION
	logMsg("Waiting for DHCP");
	osEvent event;
	do {
		event = osSignalWait(DHCP_FINISHED_SIGNAL, osWaitForever);
	} while (event.status != osOK && event.status != osEventSignal);
	logMsg("DHCP task done");
	logMsg("Terminating dhcp");
	osThreadTerminate(dhcpTaskHandle);

//INIT TASKS
	/* THREADS, QUEUES, MutexS, MUTEXES  INITIALIZATION */
	logMsg("Initializing pools");
	amplitudePool_id = osPoolCreate(osPool(amplitudePool));
	if (amplitudePool_id == NULL)
		printHandleNull("Amp pool");
	cfftPool_id = osPoolCreate(osPool(cfftPool));
	if (cfftPool_id == NULL)
		printHandleNull("Cfft pool");
	soundBufferPool_id = osPoolCreate(osPool(soundBufferPool));
	if (soundBufferPool_id == NULL)
		printHandleNull("Sound buff pool");
	stmConfigPool_id = osPoolCreate(osPool(stmConfigPool));
	if (stmConfigPool_id == NULL)
		printHandleNull("Stm config pool");

	logMsg("Initializing mail queues");
	audioMail_q_id = osMailCreate(osMailQ(audioMail_q), NULL);
	if (audioMail_q_id == NULL)
		printHandleNull("Audio mail q");

	logMsg("Initializing mutexes");
	sharedAmplitudeMutex_id = osMutexCreate(osMutex(sharedAmplitudeMutex));
	if (sharedAmplitudeMutex_id == NULL)
		printHandleNull("Lcd freq mut");
	samplingMutex_id = osMutexCreate(osMutex(samplingMutex));
	if (samplingMutex_id == NULL)
		printHandleNull("Samp mut");
	ethernetMutex_id = osMutexCreate(osMutex(ethernetMutex));
	if (ethernetMutex_id == NULL)
		printHandleNull("Eth mut");

	logMsg("Preparing global variables");
	/* PREPARE GLOBAL VARIABLES */
	configStr = osPoolCAlloc(stmConfigPool_id);
	sharedAmplitude = osPoolCAlloc(amplitudePool_id);
	mainSoundBuffer = osPoolCAlloc(soundBufferPool_id);
	mainSoundBuffer->iterator = 0;
	mainSoundBuffer->frequency = audioSamplingFrequency;
	mainSoundBuffer->soundBufferSize = SOUND_BUFFER_MAX_BUFFER_SIZE;
	for (uint32_t i = 0; i < mainSoundBuffer->soundBufferSize; i++) {
		mainSoundBuffer->soundBuffer[i] = 0;
	}
	configStr->amplitudeSamplingDelay = CONNECTION_TASK_DELAY_TIME;

	logMsg("Preparing audio recording");
	/* Audio recorder - initialization */
	if (audioRecorderInit(AUDIO_RECORDER_INPUT_MICROPHONE,
	AUDIO_RECORDER_VOLUME_0DB, audioSamplingFrequency) != AUDIO_RECORDER_OK) {
		logErr("Audio rec init");
	} else {
		logMsg("Audio rec init");
	}

	/* Audio recorder - recording */
	if (audioRecorderStartRecording(temporaryAudioBuffer,
	AUDIO_BUFFER_SIZE) != AUDIO_RECORDER_OK) {
		logErr("Audio buffer start");
	} else {
		logMsg("Audio buffer start");
	}

	logMsg("Initializing tasks");
#ifdef LCD_PRINTER_SUPPORT
	lcdTaskHandle = osThreadCreate(osThread(lcdThread), NULL);
	if (lcdTaskHandle == NULL)
	printHandleNull("Lcd task");
#endif
	soundProcessingTaskHandle = osThreadCreate(osThread(soundProcessingThread),
	NULL);
	if (soundProcessingTaskHandle == NULL)
		printHandleNull("SP task");
	samplingTaskHandle = osThreadCreate(osThread(samplingThread), NULL);
	if (samplingTaskHandle == NULL)
		printHandleNull("Samp task");
	streamingTaskHandle = osThreadCreate(osThread(streamingThread), NULL);
	if (streamingTaskHandle == NULL)
		printHandleNull("Conn task");
	httpConfigTaskHandle = osThreadCreate(osThread(httpConfigThread), NULL);
	if (httpConfigTaskHandle == NULL)
		printHandleNull("HTTP task");

	logMsg("Terminating init");
	osThreadTerminate(initTaskHandle);
}

void dhcpTask(void const * argument) {
	if (isEthernetCableConnected()) {
		logMsg("Ethernet cable is connected");
		ip_addr_t ipaddr;
		ip_addr_t netmask;
		ip_addr_t gw;
		uint32_t IPaddress;
		DHCP_state = DHCP_START;
		do {
			switch (DHCP_state) {
			case DHCP_START : {
				gnetif.ip_addr.addr = 0;
				gnetif.netmask.addr = 0;
				gnetif.gw.addr = 0;
				IPaddress = 0;
				dhcp_start(&gnetif);
				DHCP_state = DHCP_WAIT_ADDRESS;
				logMsg("Looking for DHCP server ...");
			}
				break;

			case DHCP_WAIT_ADDRESS : {

				IPaddress = gnetif.ip_addr.addr;

				if (IPaddress != 0) {
					DHCP_state = DHCP_ADDRESS_ASSIGNED;

					dhcp_stop(&gnetif);

					logMsg("DHCP IP");
					printAddress(&gnetif, IP_ADDRESS);
					printAddress(&gnetif, NETMASK_ADDRESS);
					printAddress(&gnetif, GATEWAY_ADDRESS);
				} else {
					if (gnetif.dhcp->tries > MAX_DHCP_TRIES) {
						DHCP_state = DHCP_TIMEOUT;

						dhcp_stop(&gnetif);

						IP4_ADDR(&ipaddr, IP_ADDR0, IP_ADDR1, IP_ADDR2,
								IP_ADDR3);
						IP4_ADDR(&netmask, NETMASK_ADDR0, NETMASK_ADDR1,
								NETMASK_ADDR2, NETMASK_ADDR3);
						IP4_ADDR(&gw, GW_ADDR0, GW_ADDR1, GW_ADDR2, GW_ADDR3);
						netif_set_addr(&gnetif, &ipaddr, &netmask, &gw);

						logErr("DHCP timeout!");
						logMsg("Static IP");
						printAddress(&gnetif, IP_ADDRESS);
						printAddress(&gnetif, NETMASK_ADDRESS);
						printAddress(&gnetif, GATEWAY_ADDRESS);
					}
				}
			}
				break;

			default:
				break;
			}
			osDelay(250);
		} while (DHCP_state != DHCP_TIMEOUT
				&& DHCP_state != DHCP_ADDRESS_ASSIGNED );
	} else {
		dhcp_stop(&gnetif);
		logErr("Ethernet cable is not connected");
		logMsg("Static IP");
		printAddress(&gnetif, IP_ADDRESS);
		printAddress(&gnetif, NETMASK_ADDRESS);
		printAddress(&gnetif, GATEWAY_ADDRESS);
	}

//SENDING DHCP FINISHED SIGNAL
	osStatus status = osSignalSet(initTaskHandle, DHCP_FINISHED_SIGNAL);
	if (status != osOK) {
		logMsgVal("ERROR: DHCP finished signal error ", status);
	}

	while (1)
		osDelay(osWaitForever);
}

void audioRecorder_FullBufferFilled(void) {
	SoundMail *soundSamples;
	soundSamples = osMailAlloc(audioMail_q_id, osWaitForever);
	audioRecordingSoundMailFill(soundSamples, temporaryAudioBuffer,
	AUDIO_BUFFER_SIZE, audioSamplingFrequency);
	osStatus status = osMailPut(audioMail_q_id, soundSamples);
	if (status != osOK)
		logErrVal("Buffer filled ERROR ", status);
}

void samplingTask(void const * argument) {
	while (1) {
		osEvent event = osMailGet(audioMail_q_id, osWaitForever);
		if (event.status == osEventMail) {
			SoundMail *receivedSound = (SoundMail *) event.value.p;
			osStatus status = osMutexWait(samplingMutex_id, osWaitForever);
			if (status == osOK) {
				audioRecordingUpdateSoundBuffer(mainSoundBuffer, receivedSound);
				status = osMutexRelease(samplingMutex_id);
				if (status != osOK) {
					logErrVal("Sampling mutex released error ", status);
				}
			} else {
				logErr("Sampling mutex error");
			}
			status = osMailFree(audioMail_q_id, receivedSound);
			if (status != osOK) {
				logErrVal("Sound mail free error ", status);
			}
		}
	}
}

void soundProcessingTask(void const * argument) {
	AmplitudeStr* amplitudeStr;
	arm_cfft_instance_f32* cfftInstance;
	osStatus status;
	osEvent event;

	amplitudeStr = osPoolCAlloc(amplitudePool_id);
	cfftInstance = osPoolCAlloc(cfftPool_id);

	while (1) {
		event = osSignalWait(START_SOUND_PROCESSING_SIGNAL, osWaitForever);

		if (event.status == osEventSignal) {

			status = osMutexWait(samplingMutex_id, osWaitForever);
			if (status == osOK) {

				soundProcessingGetCfftInstance(cfftInstance,
						mainSoundBuffer->soundBufferSize / 2);

				if (cfftInstance != NULL) {
					float32_t processingBuffer[SOUND_BUFFER_MAX_BUFFER_SIZE];
					soundProcessingAmplitudeInit(amplitudeStr, mainSoundBuffer,
							processingBuffer);
					soundProcessingGetAmplitudeInstance(cfftInstance,
							amplitudeStr, processingBuffer);
					status = osMutexRelease(samplingMutex_id);
					if (status != osOK) {
						logErrVal("Sampling mutex (sound processing) released ",
								status);
					}

					status = osMutexWait(sharedAmplitudeMutex_id,
					osWaitForever);
					if (status == osOK) {

						soundProcessingCopyAmplitudeInstance(amplitudeStr,
								sharedAmplitude);
						status = osMutexRelease(sharedAmplitudeMutex_id);
						if (status != osOK) {
							logErrVal("Shared amp mutex released error ",
									status);
						}
					} else {
						logErrVal("Shared amp mutex wait error: ", status);
					}

				} else {
					logErr("Cfft NULL error");
					status = osMutexRelease(samplingMutex_id);
					if (status != osOK) {
						logErrVal(
								"Sampling mutex (sound processing) released error ",
								status);
					}
				}
			} else {
				logErr("Sampling mutex (sound processing) error");
			}
		} else
			logErrVal("ST sp wait", event.status);
	}
}

#ifdef LCD_PRINTER_SUPPORT
void lcdTask(void const * argument) {
	while (1) {
		osDelay(LCD_TASK_DELAY_TIME);
		osStatus status = osMutexWait(sharedAmplitudeMutex_id, osWaitForever);
		if (status == osOK) {
			lcdAmpPrinterPrint(&sharedAmplitude);
			status = osMutexRelease(sharedAmplitudeMutex_id);
			if (status != osOK) {
				logErrVal("lcdFrequencyMutex released error ", status);
			}
		}
	}
}
#endif

void streamingTask(void const * argument) {
	struct netconn *udpServer = NULL;
	err_t status;
	err_t netErr;

	ip_addr_t clientIp;
	IP4_ADDR(&clientIp, 192, 168, 0, 10);

	udpServer = netconn_new(NETCONN_UDP);
	if (udpServer == NULL)
		logErr("Udp server is null");
	else
		udpServer->recv_timeout = 1;

	status = netconn_bind(udpServer, &gnetif.ip_addr, 53426);
	if (status != ERR_OK)
		logErrVal("Udp bind error", status);

	while (1) {
		status = osSignalSet(soundProcessingTaskHandle,
		START_SOUND_PROCESSING_SIGNAL);
		osDelay(configStr->amplitudeSamplingDelay);

		osStatus status = osMutexWait(ethernetMutex_id, osWaitForever);
		if (status == osOK) {

			status = osMutexWait(sharedAmplitudeMutex_id,
			osWaitForever);
			if (status == osOK) {
				netErr = netconn_connect(udpServer, &clientIp, 53426);
				if (netErr)
					logErrVal("Connect err ", netErr);
				netErr = amplitudeSend(sharedAmplitude, udpServer);
				if (netErr)
					logErrVal("Write ", netErr);
				status = osMutexRelease(sharedAmplitudeMutex_id);
				if (status != osOK)
					logErrVal("Lcd mut eth rel ", status);
			} else {
				logErrVal("Lcd mut eth wait ", status);
			}

			status = osMutexRelease(ethernetMutex_id);
			if (status != osOK)
				logErrVal("Ethmut release ", status);
		}
	}
}

void httpConfigTask(void const* argument) {
	struct netconn *httpServer = NULL;
	httpServer = netconn_new(NETCONN_TCP);

	if (httpServer == NULL)
		logErr("Tcp server is null");
	httpServer->recv_timeout = HTTP_HOST_ACCEPT_TIMEOUT;

	err_t netStatus = netconn_bind(httpServer, &gnetif.ip_addr, 80);
	if (netStatus != ERR_OK)
		logErrVal("TCP bind error", netStatus);

	netStatus = netconn_listen(httpServer);
	if (netStatus != ERR_OK)
		logErrVal("TCP listen error", netStatus);

	while (1) {
		osDelay(HTTP_CONFIG_TASK_DELAY_TIME);
		logMsg("HTTP task");
		osStatus status = osMutexWait(ethernetMutex_id, osWaitForever);
		if (status == osOK) {
			struct netconn *newClient = NULL;
			netStatus = netconn_accept(httpServer, &newClient);
			if (netStatus == ERR_OK) {
				logMsg("New client");

				struct netbuf* recvBuf;
				newClient->recv_timeout = HTTP_RECEIVE_TIMEOUT;
				err_t netStatus = netconn_recv(newClient, &recvBuf);
				if (netStatus == ERR_OK) {
					uint16_t requestType = getRequestType(recvBuf);
					logMsgVal("Request type ", requestType);
					if (requestType == GET_REQUEST) {
						if (isConfigRequest(recvBuf)) {
							sendConfiguration(configStr, newClient);
						}
					} else {

						if (requestType == PUT_REQUEST) {
							netbuf_delete(recvBuf);

							sendConfiguration(configStr, newClient);

							err_t netStatus = netconn_recv(newClient, &recvBuf);
							if (netStatus == ERR_OK) {
								StmConfig config;
								parse(recvBuf, &config);
								configCopy(configStr, &config);
							}
						}
					}
					netbuf_delete(recvBuf);
				} else
					logErrVal("No data ", netStatus);

				netStatus = netconn_close(newClient);
				if (netStatus != ERR_OK)
					logErrVal("TCP close ", netStatus);
				netStatus = netconn_delete(newClient);
				if (netStatus != ERR_OK)
					logErrVal("TCP delete ", netStatus);
			} else if (netStatus != ERR_TIMEOUT)
				logErrVal("TCP accept ", status);

			status = osMutexRelease(ethernetMutex_id);
			if (status != osOK)
				logErrVal("Ethmut release ", status);
		}
	}
}
