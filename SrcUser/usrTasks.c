/*
 * usrTasks.c
 *
 *  Created on: 14 wrz 2016
 *      Author: Patryk Kotlarz
 */

#include "usrTasks.h"

/**
 * @var uint8_t dhcpState
 * @brief State of DHCP client
 */
extern uint8_t dhcpState;

/**
 * @var netif ethernetInterfaceHandler
 * @brief Ethernet interface handler (used for DHCP searching, UDP and TCO communication)
 */
extern struct netif ethernetInterfaceHandler;

//StmConfig* configStr;

/**
 * @var SoundBuffer* mainSoundBuffer
 * @brief Cyclic buffer which holds audio samples
 */
SoundBufferStr* mainSoundBuffer;

/**
 * @var uint16_t dmaAudioBuffer[AUDIO_BUFFER_SIZE]
 * @brief Buffer which holds samples from last DMA interrupt
 */
uint16_t dmaAudioBuffer[AUDIO_BUFFER_SIZE];

/**
 * @var uint32_t audioSamplingFrequency
 * @brief Audio samping frequency
 */
uint32_t audioSamplingFrequency = 44100;

/**
 * @var AmplitudeStr* mainSpectrumBuffer
 * @brief Buffer which holds spectrum samples
 */
SpectrumStr* mainSpectrumBuffer;

/* Task handlers */
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

osThreadId dhcpInitTaskHandle;
osThreadDef(dhcpInitThread, dhcpTask, osPriorityNormal, 1,
		5*configMINIMAL_STACK_SIZE);

osThreadId soundProcessingTaskHandle;
osThreadDef(soundProcessingThread, soundProcessingTask, osPriorityHigh, 1,
		195*configMINIMAL_STACK_SIZE);

/* Memory pool handlers */
osPoolDef(soundBufferPool, 1, SoundBufferStr);
osPoolId soundBufferPool_id;
osPoolDef(spectrumBufferPool, 2, SpectrumStr);
osPoolId spectrumBufferPool_id;
osPoolDef(cfftPool, 1, arm_cfft_instance_f32);
osPoolId cfftPool_id;
osPoolDef(soundProcessingBufferPool, 1,
		float32_t[MAIN_SOUND_BUFFER_MAX_BUFFER_SIZE]);
osPoolId soundProcessingBufferPool_id;

//osPoolDef(stmConfigPool, 1, StmConfig);
//osPoolId stmConfigPool_id;

/* Mail queue handler */
osMailQDef(dmaAudioMail_q, MAXIMUM_DMA_AUDIO_MESSAGE_QUEUE_SIZE, SoundMailStr);
osMailQId dmaAudioMail_q_id;

/* Mutex handlers */
osMutexDef(mainSpectrumBufferMutex);
osMutexId mainSpectrumBufferMutex_id;

osMutexDef(ethernetInterfaceMutex);
osMutexId ethernetInterfaceMutex_id;

osMutexDef(mainSoundBufferMutex);
osMutexId mainSoundBufferMutex_id;

// FUNCTIONS

/**
 * @brief Function initializes the initialization task
 */
void threadsInit() {
	/* Starting OS task initialization */
	initTaskHandle = osThreadCreate(osThread(initThread), NULL);
}

// OS TASKS

/**
 * @brief Initialization task
 */
void initTask(void const * argument) {
	/* PERIPHERALS INITIALIZATION */
	MX_FMC_Init();
	MX_LTDC_Init();
	MX_DMA2D_Init();
	MX_GPIO_Init();
	lcdInit();
	logMsg("Ethernet initialization...");
	MX_LWIP_Init();

	/* DHCP initialization */
	dhcpInitTaskHandle = osThreadCreate(osThread(dhcpInitThread), NULL);

	logMsg("Waiting for DHCP");
	osEvent event;
	do {
		// waiting for DHCP initialization
		event = osSignalWait(DHCP_FINISHED_SIGNAL, osWaitForever);
	} while (event.status != osOK && event.status != osEventSignal);
	logMsg("DHCP task done");

	// terminating DHCP initialization task
	logMsg("Terminating dhcp");
	osThreadTerminate(dhcpInitTaskHandle);

	/* Taska, mutexes, mail queues and memory pools initialization */
	logMsg("Initializing memory pools");
	spectrumBufferPool_id = osPoolCreate(osPool(spectrumBufferPool));
	if (spectrumBufferPool_id == NULL)
		printNullHandle("Spect pool");
	cfftPool_id = osPoolCreate(osPool(cfftPool));
	if (cfftPool_id == NULL)
		printNullHandle("Cfft pool");
	soundBufferPool_id = osPoolCreate(osPool(soundBufferPool));
	if (soundBufferPool_id == NULL)
		printNullHandle("Sound pool");
	/*stmConfigPool_id = osPoolCreate(osPool(stmConfigPool));
	 if (stmConfigPool_id == NULL)
	 printHandleNull("Stm config pool");*/

	logMsg("Initializing mail queues");
	dmaAudioMail_q_id = osMailCreate(osMailQ(dmaAudioMail_q), NULL);
	if (dmaAudioMail_q_id == NULL)
		printNullHandle("Audio mail q");

	logMsg("Initializing mutexes");
	mainSpectrumBufferMutex_id = osMutexCreate(
			osMutex(mainSpectrumBufferMutex));
	if (mainSpectrumBufferMutex_id == NULL)
		printNullHandle("Spect mut");
	mainSoundBufferMutex_id = osMutexCreate(osMutex(mainSoundBufferMutex));
	if (mainSoundBufferMutex_id == NULL)
		printNullHandle("Audio mut");
	ethernetInterfaceMutex_id = osMutexCreate(osMutex(ethernetInterfaceMutex));
	if (ethernetInterfaceMutex_id == NULL)
		printNullHandle("Eth mut");

	/* Global variables */
	logMsg("Preparing global variables");
	//configStr = osPoolCAlloc(stmConfigPool_id);
	mainSpectrumBuffer = osPoolCAlloc(spectrumBufferPool_id);
	mainSoundBuffer = osPoolCAlloc(soundBufferPool_id);
	mainSoundBuffer->iterator = 0;
	mainSoundBuffer->frequency = audioSamplingFrequency;
	mainSoundBuffer->size = MAIN_SOUND_BUFFER_MAX_BUFFER_SIZE;
	for (uint32_t i = 0; i < mainSoundBuffer->size; i++) {
		mainSoundBuffer->soundBuffer[i] = 0;
	}
	//configStr->amplitudeSamplingDelay = CONNECTION_TASK_DELAY_TIME;

	logMsg("Preparing audio recording");
	if (audioRecorderInit(AUDIO_RECORDER_INPUT_MICROPHONE,
	AUDIO_RECORDER_VOLUME_0DB, audioSamplingFrequency) != AUDIO_RECORDER_OK) {
		logErr("Audio rec init");
	}

	/* Audio recorder - start recording */
	if (audioRecorderStartRecording(dmaAudioBuffer,
	AUDIO_BUFFER_SIZE) != AUDIO_RECORDER_OK) {
		logErr("Audio buffer start");
	}

	logMsg("Initializing tasks");
#ifdef LCD_PRINTER_SUPPORT
	lcdTaskHandle = osThreadCreate(osThread(lcdThread), NULL);
	if (lcdTaskHandle == NULL)
	printNullHandle("Lcd task");
#endif
	soundProcessingTaskHandle = osThreadCreate(osThread(soundProcessingThread),
	NULL);
	if (soundProcessingTaskHandle == NULL)
		printNullHandle("Sound proc task");
	samplingTaskHandle = osThreadCreate(osThread(samplingThread), NULL);
	if (samplingTaskHandle == NULL)
		printNullHandle("Samp task");
	streamingTaskHandle = osThreadCreate(osThread(streamingThread), NULL);
	if (streamingTaskHandle == NULL)
		printNullHandle("Stream task");
	httpConfigTaskHandle = osThreadCreate(osThread(httpConfigThread), NULL);
	if (httpConfigTaskHandle == NULL)
		printNullHandle("HTTP task");

	logMsg("Terminating init");
	osThreadTerminate(initTaskHandle);
}

/**
 * @brief DHCP initialization task
 */
void dhcpTask(void const * argument) {
	if (isEthernetCableConnected()) {
		logMsg("Ethernet cable is connected");
		uint32_t ipAddress;
		dhcpState = DHCP_START;
		do {
			switch (dhcpState) {
			case DHCP_START : {
				ethernetInterfaceHandler.ip_addr.addr = 0;
				ethernetInterfaceHandler.netmask.addr = 0;
				ethernetInterfaceHandler.gw.addr = 0;
				ipAddress = 0;
				dhcp_start(&ethernetInterfaceHandler);
				dhcpState = DHCP_WAIT_ADDRESS;
				logMsg("Looking for DHCP server ...");
				break;
			}

			case DHCP_WAIT_ADDRESS : {

				ipAddress = ethernetInterfaceHandler.ip_addr.addr;

				if (ipAddress != 0) {
					dhcpState = DHCP_ADDRESS_ASSIGNED;

					dhcp_stop(&ethernetInterfaceHandler);

					logMsg("Got IP by DHCP:");
					printAddress(&ethernetInterfaceHandler, IP_ADDRESS);
					printAddress(&ethernetInterfaceHandler, NETMASK_ADDRESS);
					printAddress(&ethernetInterfaceHandler, GATEWAY_ADDRESS);
				} else {
					if (ethernetInterfaceHandler.dhcp->tries > MAX_DHCP_TRIES) {
						dhcpState = DHCP_TIMEOUT;

						dhcp_stop(&ethernetInterfaceHandler);

						ip_addr_t ipaAddress;
						ip_addr_t netmask;
						ip_addr_t gateway;

						IP4_ADDR(&ipaAddress, IP_ADDR0, IP_ADDR1, IP_ADDR2,
								IP_ADDR3);
						IP4_ADDR(&netmask, NETMASK_ADDR0, NETMASK_ADDR1,
								NETMASK_ADDR2, NETMASK_ADDR3);
						IP4_ADDR(&gateway, GW_ADDR0, GW_ADDR1, GW_ADDR2,
								GW_ADDR3);
						netif_set_addr(&ethernetInterfaceHandler, &ipaAddress,
								&netmask, &gateway);

						logErr("DHCP timeout!");
						logMsg("Static IP");
						printAddress(&ethernetInterfaceHandler, IP_ADDRESS);
						printAddress(&ethernetInterfaceHandler,
						NETMASK_ADDRESS);
						printAddress(&ethernetInterfaceHandler,
						GATEWAY_ADDRESS);
					}
				}
			}
				break;

			default:
				break;
			}
			osDelay(250);
		} while (dhcpState != DHCP_TIMEOUT && dhcpState != DHCP_ADDRESS_ASSIGNED );
	} else {
		dhcp_stop(&ethernetInterfaceHandler);
		logErr("Ethernet cable is not connected");
		logMsg("Static IP");
		printAddress(&ethernetInterfaceHandler, IP_ADDRESS);
		printAddress(&ethernetInterfaceHandler, NETMASK_ADDRESS);
		printAddress(&ethernetInterfaceHandler, GATEWAY_ADDRESS);
	}

	// sending DHCP finished signal to initialization task
	osStatus status = osSignalSet(initTaskHandle, DHCP_FINISHED_SIGNAL);
	if (status != osOK) {
		logErrVal("ERROR: DHCP finished signal", status);
	}

	// waiting forever for termination
	while (1)
		osDelay(osWaitForever);
}

/**
 * @brief Functions called as DMA interrupt
 */
void audioRecorder_FullBufferFilled(void) {
	SoundMailStr *soundSamples;

	// allocating memory for sound mail
	soundSamples = osMailAlloc(dmaAudioMail_q_id, osWaitForever);
	audioRecordingSoundMailFill(soundSamples, dmaAudioBuffer,
	AUDIO_BUFFER_SIZE, audioSamplingFrequency);

	// sending mail to queue
	osStatus status = osMailPut(dmaAudioMail_q_id, soundSamples);
	if (status != osOK)
		logErrVal("Audio mail send", status);
}

/**
 * @brief Asynchronous task which gets audio mails from queue and fills the mainSoundBuffer
 */
void samplingTask(void const * argument) {
	while (1) {
		// waiting for new mail
		osEvent event = osMailGet(dmaAudioMail_q_id, osWaitForever);
		if (event.status == osEventMail) {
			SoundMailStr *receivedSound = (SoundMailStr *) event.value.p;

			// waiting for access to mailSoundBuffer
			osStatus status = osMutexWait(mainSoundBufferMutex_id,
			osWaitForever);
			if (status == osOK) {
				// filling cyclic buffer
				audioRecordingUpdateSoundBuffer(mainSoundBuffer, receivedSound);

				// releasing mutex
				status = osMutexRelease(mainSoundBufferMutex_id);
				if (status != osOK) {
					logErrVal("Sampling mutex release", status);
				}
			} else {
				logErr("Sampling mutex");
			}

			// free audio mail memory
			status = osMailFree(dmaAudioMail_q_id, receivedSound);
			if (status != osOK) {
				logErrVal("Sound mail free", status);
			}
		}
	}
}

/**
 * @brief FFT processing task
 */
void soundProcessingTask(void const * argument) {
	SpectrumStr* temporarySpectrumBufferStr;
	arm_cfft_instance_f32* cfftInstance;
	osStatus status;
	osEvent event;

	// allocating memory for temporary spectrum buffer
	temporarySpectrumBufferStr = osPoolCAlloc(spectrumBufferPool_id);
	cfftInstance = osPoolCAlloc(cfftPool_id);

	while (1) {
		// waiting for start signal
		event = osSignalWait(START_SOUND_PROCESSING_SIGNAL, osWaitForever);

		if (event.status == osEventSignal) {

			// waiting for access to main sound buffer
			status = osMutexWait(mainSoundBufferMutex_id, osWaitForever);
			if (status == osOK) {

				// getting FFT instance
				soundProcessingGetCfftInstance(cfftInstance,
						mainSoundBuffer->size / 2);

				if (cfftInstance != NULL) {
					float32_t temporaryAudioBuffer[MAIN_SOUND_BUFFER_MAX_BUFFER_SIZE];

					// spectrum buffer initialization and sound buffer copying
					soundProcessingAmplitudeInit(temporarySpectrumBufferStr,
							mainSoundBuffer, temporaryAudioBuffer);

					// releasing mainSoundBufferMutex
					status = osMutexRelease(mainSoundBufferMutex_id);
					if (status != osOK) {
						logErrVal("Sampling mutex (sound processing) release",
								status);
					}

					// calculating spectrum
					soundProcessingGetAmplitudeInstance(cfftInstance,
							temporarySpectrumBufferStr, temporaryAudioBuffer);

					// waiting for access to main spectrum buffer
					status = osMutexWait(mainSpectrumBufferMutex_id,
					osWaitForever);
					if (status == osOK) {

						// copying spectrum from temporary buffer to main buffer
						soundProcessingCopyAmplitudeInstance(
								temporarySpectrumBufferStr, mainSpectrumBuffer);

						// releasing main spectrum buffer mutex
						status = osMutexRelease(mainSpectrumBufferMutex_id);
						if (status != osOK) {
							logErrVal("Shared amp mutex released", status);
						}
					} else {
						logErrVal("Shared amp mutex wait", status);
					}

				} else {
					logErr("Cfft NULL");

					// releasing main sound buffer mutex
					status = osMutexRelease(mainSoundBufferMutex_id);
					if (status != osOK) {
						logErrVal("Sampling mutex (sound processing) release",
								status);
					}
				}
			} else {
				logErr("Sampling mutex (sound processing)");
			}
		} else
			logErrVal("ST sp wait", event.status);
	}
}

#ifdef LCD_PRINTER_SUPPORT
void lcdTask(void const * argument) {
	while (1) {
		osDelay(LCD_TASK_DELAY_TIME);
		osStatus status = osMutexWait(mainSpectrumBufferMutex_id, osWaitForever);
		if (status == osOK) {
			lcdAmpPrinterPrint(&mainSpectrumBuffer);
			status = osMutexRelease(mainSpectrumBufferMutex_id);
			if (status != osOK) {
				logErrVal("lcdFrequencyMutex release", status);
			}
		}
	}
}
#endif

/**
 * @brief Spectrum UDP streaming
 */
void streamingTask(void const * argument) {
	struct netconn *udpStreamingSocket = NULL;
	err_t status;
	err_t netErr;

	ip_addr_t clientIp;
	IP4_ADDR(&clientIp, 192, 168, 0, 10);

	// creating UDP socket
	udpStreamingSocket = netconn_new(NETCONN_UDP);
	if (udpStreamingSocket == NULL)
		logErr("Null UDP client");
	else
		udpStreamingSocket->recv_timeout = 1;

	// binding socket to ethernet interface on UDP_STREAMING_PORT
	status = netconn_bind(udpStreamingSocket, &ethernetInterfaceHandler.ip_addr,
	UDP_STREAMING_PORT);
	if (status != ERR_OK)
		logErrVal("Udp bind", status);

	while (1) {
		// setting signal to start sound processing
		status = osSignalSet(soundProcessingTaskHandle,
		START_SOUND_PROCESSING_SIGNAL);
		//osDelay(configStr->amplitudeSamplingDelay);

		// delay
		osDelay(100);

		// waiting for acces to ethernet interface
		osStatus status = osMutexWait(ethernetInterfaceMutex_id, osWaitForever);
		if (status == osOK) {

			// waiting for access to main spectrum buffer
			status = osMutexWait(mainSpectrumBufferMutex_id,
			osWaitForever);
			if (status == osOK) {

				// "connecting" to UDP
				netErr = netconn_connect(udpStreamingSocket, &clientIp,
				UDP_STREAMING_PORT);
				if (netErr)
					logErrVal("UDP connect", netErr);

				// sending main spectrum buffer by UDP
				netErr = sendSpectrum(mainSpectrumBuffer, udpStreamingSocket);
				if (netErr)
					logErrVal("UDP write", netErr);

				// releasing main spectrum buffer mutex
				status = osMutexRelease(mainSpectrumBufferMutex_id);
				if (status != osOK)
					logErrVal("UDP main spect mut release", status);
			} else {
				logErrVal("UDP eth int mut wait", status);
			}

			// releasing ethernet interface mutex
			status = osMutexRelease(ethernetInterfaceMutex_id);
			if (status != osOK)
				logErrVal("UDP eth mut release", status);
		}
	}
}

/**
 * @brief Device configuration (by network using HTTP)
 */
void httpConfigTask(void const* argument) {
	struct netconn *httpServer = NULL;

	// creating TCP server
	httpServer = netconn_new(NETCONN_TCP);
	if (httpServer == NULL)
		logErr("Null TCP");
	httpServer->recv_timeout = HTTP_HOST_ACCEPT_TIMEOUT;

	// binding server to ethernet interface on port 80
	err_t netStatus = netconn_bind(httpServer,
			&ethernetInterfaceHandler.ip_addr, 80);
	if (netStatus != ERR_OK)
		logErrVal("TCP bind", netStatus);

	// starting listening
	netStatus = netconn_listen(httpServer);
	if (netStatus != ERR_OK)
		logErrVal("TCP listen", netStatus);

	while (1) {
		// delay
		osDelay(HTTP_CONFIG_TASK_DELAY_TIME);
		logMsg("HTTP task");

		// waiting for acces to ethernet interface
		osStatus status = osMutexWait(ethernetInterfaceMutex_id, osWaitForever);
		if (status == osOK) {
			struct netconn *newClient = NULL;

			// accepting incoming client
			netStatus = netconn_accept(httpServer, &newClient);
			if (netStatus == ERR_OK) {
				// if there is a client
				logMsg("New client");

				struct netbuf* recvBuf;
				newClient->recv_timeout = HTTP_RECEIVE_TIMEOUT;

				// receiving data from client
				err_t netStatus = netconn_recv(newClient, &recvBuf);
				if (netStatus == ERR_OK) {

					// encoding HTTP request type
					uint16_t requestType = getRequestType(recvBuf);
					logMsgVal("Request type ", requestType);
					if (requestType == GET_REQUEST) {
						// if it is GET request
						if (isConfigRequest(recvBuf)) {
							// if it is GET config request
							//sendConfiguration(configStr, newClient);
						}
					} else {
						if (requestType == PUT_REQUEST) {
							// if it is PUT request

							//???
							netbuf_delete(recvBuf);

							//sendConfiguration(configStr, newClient);

							err_t netStatus = netconn_recv(newClient, &recvBuf);
							if (netStatus == ERR_OK) {
								StmConfig config;

								// parsing JSON data to config structure
								parse(recvBuf, &config);

								// copying temporary structure to main config structure
								//configCopy(configStr, &config);
							}
						}
					}
					netbuf_delete(recvBuf);
				} else
					logErrVal("No data", netStatus);

				// closing connectoin
				netStatus = netconn_close(newClient);
				if (netStatus != ERR_OK)
					logErrVal("TCP close", netStatus);

				// free client memory
				netStatus = netconn_delete(newClient);
				if (netStatus != ERR_OK)
					logErrVal("TCP delete", netStatus);
			} else if (netStatus != ERR_TIMEOUT)
				logErrVal("TCP accept", status);

			// releasing ethernet interface mutex
			status = osMutexRelease(ethernetInterfaceMutex_id);
			if (status != osOK)
				logErrVal("Eth mut release", status);
		}
	}
}
