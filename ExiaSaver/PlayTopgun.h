void playTopgn(char * _filepath)
{
	if (_filepath == NULL) return;

	//Initialize all the character images.
	IMG * _IMG_jet = NULL;
	IMG * _IMG_sky = NULL;

	//--- Initialize and fill the image list.
	int _n = listFile(NULL, _filepath, ".pnm"); if (_n < 1) return;
	char ** _filelist = (IMG**)malloc(_n * sizeof(IMG*));
	listFile(_filelist, _filepath, ".pnm");

	for (int _i = 0; _i < _n; _i++)
	{
		char * _filename = strrchr(_filelist[_i], '\\') + 1;
		if (!strcmp(_filename, "img_jet.pnm")) _IMG_jet = imgImport(_filelist[_i]);
	};

	//--- Initialize the screen and objects.
	SCR * _SCR = scrNew();
	_OBJ_jet = objNew(_IMG_jet);
	scrAddObj(_SCR, _OBJ_jet);

	float _speed_x = 0.f;
	float _speed_y = 1.f;

	//--- Place and set each numbers
	_OBJ_jet->x = -35;

	//--- Start the render loop in an other thread.
	#if defined(_WIN32)
	CreateThread(NULL, 0, (LPTHREAD_START_ROUTINE)scrRenderRealtime, _SCR, 0, NULL);
	#else
	pthread_t _thread;
	pthread_create(&_thread, NULL, scrRenderRealtime, _SCR);
	#endif

	while (_SCR != NULL)
	{

	}
};