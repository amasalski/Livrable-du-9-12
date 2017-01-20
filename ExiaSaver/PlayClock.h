void playClock(char * _filepath)
{
	if (_filepath == NULL) return;

	//Initialize all the character images.
	IMG * _IMG_0 = NULL; IMG * _IMG_1 = NULL;
	IMG * _IMG_2 = NULL; IMG * _IMG_3 = NULL;
	IMG * _IMG_4 = NULL; IMG * _IMG_5 = NULL;
	IMG * _IMG_6 = NULL; IMG * _IMG_7 = NULL;
	IMG * _IMG_8 = NULL; IMG * _IMG_9 = NULL;
	IMG * _IMG_c = NULL;

	//--- Initialize and fill the image list.
	sprintf(_filepath, "%s/clock", _filepath);
	int _n = listFile(NULL, _filepath, ".pnm"); if (_n < 1) return;
	char ** _filelist = (IMG**)malloc(_n * sizeof(IMG*));
	listFile(_filelist, _filepath, ".pnm");

	//--- Assign each images to their corresponding variables.
	for (int _i = 0; _i < _n; _i++)
	{
		char * _filename = strrchr(_filelist[_i], '/') + 1;
		if		(!strcmp(_filename, "0.pnm")) _IMG_0 = imgImport(_filelist[_i]);
		else if (!strcmp(_filename, "1.pnm")) _IMG_1 = imgImport(_filelist[_i]);
		else if (!strcmp(_filename, "2.pnm")) _IMG_2 = imgImport(_filelist[_i]);
		else if (!strcmp(_filename, "3.pnm")) _IMG_3 = imgImport(_filelist[_i]);
		else if (!strcmp(_filename, "4.pnm")) _IMG_4 = imgImport(_filelist[_i]);
		else if (!strcmp(_filename, "5.pnm")) _IMG_5 = imgImport(_filelist[_i]);
		else if (!strcmp(_filename, "6.pnm")) _IMG_6 = imgImport(_filelist[_i]);
		else if (!strcmp(_filename, "7.pnm")) _IMG_7 = imgImport(_filelist[_i]);
		else if (!strcmp(_filename, "8.pnm")) _IMG_8 = imgImport(_filelist[_i]);
		else if (!strcmp(_filename, "9.pnm")) _IMG_9 = imgImport(_filelist[_i]);
		else if (!strcmp(_filename, "c.pnm")) _IMG_c = imgImport(_filelist[_i]);
	};

	//--- Initialize the screen and objects.
	SCR * _SCR = scrNew();
	for (int _i = 0; _i < 8; _i++) scrAddObj(_SCR, objNew(_IMG_0));
	scrAddObj(_SCR, objNew(imgGenFill((RGB) { 1.f, 0.f, 1.f }, 128, 64)));
	_SCR->OBJ[8]->o = 0.01f;

	//--- Place and set each numbers
	_SCR->OBJ[0]->x = -35;
	_SCR->OBJ[1]->x = -25;
	_SCR->OBJ[2]->x = -15;
	_SCR->OBJ[3]->x = -5;
	_SCR->OBJ[4]->x = 5;
	_SCR->OBJ[5]->x = 15;
	_SCR->OBJ[6]->x = 25;
	_SCR->OBJ[7]->x = 35;

	int _input;

	//--- Start the render loop in an other thread.
	#if defined(_WIN32)
	//CreateThread(NULL, 0, (LPTHREAD_START_ROUTINE)scrRenderRealtime, _SCR, 0, NULL);
	CreateThread(NULL, 0, (LPTHREAD_START_ROUTINE)registerInput, &_input, 0, NULL);
	#else
	pthread_t _thread;
	pthread_create(&_thread, NULL, scrRenderRealtime, _SCR);
	pthread_create(&_thread, NULL, registerInput, &_input);
	#endif

	//--- Startup opacity to 0.
	for (int _i = 0; _i < 9; _i++) _SCR->OBJ[_i]->o = 0.f;

	while (_SCR != NULL)
	{
		//--- Get time as a string.
		time_t mytime = time(NULL);
		char _timestr[9]; strftime(_timestr, 9, "%H:%M:%S", localtime(&mytime));

		//--- Increase opacity.
		for (int _i = 0; _i < 8; _i++) if (_SCR->OBJ[_i]->o < 2.f) _SCR->OBJ[_i]->o += 0.01f;
		if (_SCR->OBJ[8]->o < 0.1f) _SCR->OBJ[8]->o += 0.001f;

		//--- Text to image.
		for (int _i = 0; _i < 8; _i++)
			switch (_timestr[_i])
			{
				case '0': _SCR->OBJ[_i]->IMG = _IMG_0; break;
				case '1': _SCR->OBJ[_i]->IMG = _IMG_1; break;
				case '2': _SCR->OBJ[_i]->IMG = _IMG_2; break;
				case '3': _SCR->OBJ[_i]->IMG = _IMG_3; break;
				case '4': _SCR->OBJ[_i]->IMG = _IMG_4; break;
				case '5': _SCR->OBJ[_i]->IMG = _IMG_5; break;
				case '6': _SCR->OBJ[_i]->IMG = _IMG_6; break;
				case '7': _SCR->OBJ[_i]->IMG = _IMG_7; break;
				case '8': _SCR->OBJ[_i]->IMG = _IMG_8; break;
				case '9': _SCR->OBJ[_i]->IMG = _IMG_9; break;
				case ':': _SCR->OBJ[_i]->IMG = _IMG_c; break;
			};

		if (_input != 6) exit(0);

		sleep(100);
	};
};