void playDiapo(char * _filepath)
{
	if (_filepath == NULL) return;

	//--- Initialize the image list.
	int _n = listFile(NULL, _filepath, ".pnm");					if (_n < 1) return;
	char ** _filelist = (char**)malloc(_n * sizeof(char*));		if (_filelist == NULL) return;
	IMG ** _IMG_list = (IMG**)malloc(_n * sizeof(IMG*));		if (_IMG_list == NULL) return;

	//--- Fill the image list.
	listFile(_filelist, _filepath, ".pnm");
	for (int _i = 0; _i < _n; _i++) _IMG_list[_i] = imgImport(_filelist[_i]);

	//--- Initialize the screen and objects.
	SCR * _SCR = scrNew();
	scrAddObj(_SCR, objNew(NULL));
	scrAddObj(_SCR, objNew(NULL));

	//--- Loop the rendering function in a thread.
	#if defined(_WIN32)
	CreateThread(NULL, 0, (LPTHREAD_START_ROUTINE)scrRenderRealtime, _SCR, 0, NULL);
	#else
	pthread_t _thread;
	pthread_create(&_thread, NULL, scrRenderRealtime, _SCR);
	#endif

	while (_SCR != NULL)
	{
		//--- Select a new image and push the last one in the background.
		_SCR->OBJ[0]->IMG = _SCR->OBJ[1]->IMG;
		_SCR->OBJ[1]->IMG = _IMG_list[rand() % _n];

		//--- Increment foreground opacity from 0 to 1.
		for (_SCR->OBJ[1]->o = 0.f; _SCR->OBJ[1]->o < 1.f; _SCR->OBJ[1]->o += 0.001f) sleep(20);

		sleep(2000);
	};
};
