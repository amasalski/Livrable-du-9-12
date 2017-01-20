#ifndef _INC_ENGINE
#define _INC_ENGINE

/*--------------------------------------------------------------------------
 STRUCTURE
--------------------------------------------------------------------------*/
typedef struct IMG
{
	int w, h;			//--- Width and height of the image.
	RGB * RGB;			//--- Array color of the image data.
} IMG;
typedef struct OBJ
{
	int x, y;		//--- Position of the object.
	int w, h;		//--- Width and height of object.
	float o;		//--- Opacity of the object.
	IMG * IMG;		//--- Texture of the object.
} OBJ;
typedef struct SCR
{
	int x, y;		//--- Camera position
	int w, h;		//(-- Screen size
	int n;			//--- Obj count
	OBJ ** OBJ;		//--- Objs
} SCR;

/*--------------------------------------------------------------------------
 PROTOTYPE
--------------------------------------------------------------------------*/
//--- Object loading
IMG *	imgNew(RGB * _RGB, int _w, int _h);
IMG *	imgImport(char * _filepath);
OBJ *	objNew(IMG * _IMG);

//--- Object Rendering
SCR *	scrNew();
void	scrDraw(SCR * _SCR, RGB _RGB);
void	scrRender(SCR * _SCR);

/*--------------------------------------------------------------------------
 MISC
--------------------------------------------------------------------------*/
void setCursorPos(int _x, int _y)
{
	#if defined (_WIN32)  
	SetConsoleCursorPosition(GetStdHandle(STD_OUTPUT_HANDLE), (COORD){ _x, _y });
	#else
	printf("\e[%d;%dH", _y, _x);
	#endif
};
void showCursor(int _bool)
{
	//--- Windows code.
	#if defined(_WIN32)

	//--- Apply the new settings to the console buffer.
	CONSOLE_CURSOR_INFO _buffer;
	GetConsoleCursorInfo(GetStdHandle(STD_OUTPUT_HANDLE), &_buffer);
	SetConsoleCursorInfo(GetStdHandle(STD_OUTPUT_HANDLE), &(CONSOLE_CURSOR_INFO) { _buffer.dwSize, _bool });

	//--- UNIX code.
	#else

	//--- Use escape code to change settings.
	printf(!_bool ? "\e[?25h" : "\e[?25l");

	#endif
}
void setConsoleSize(int _w, int _h)
{
	//--- Windows code.
	#if defined(_WIN32)

	//--- Set value to the current console buffer.
	CONSOLE_SCREEN_BUFFER_INFO _buffer;
	GetConsoleScreenBufferInfo(GetStdHandle(STD_OUTPUT_HANDLE), &_buffer);
	//_w = _buffer.srWindow.Right - _buffer.srWindow.Left;
	//_h = _buffer.srWindow.Bottom - _buffer.srWindow.Top;

	//--- UNIX code.
	#else

	//--- Set value to the current console buffer.
	struct winsize _buffer;
	ioctl(STDOUT_FILENO, TIOCGWINSZ, &_buffer);
	_buffer.ws_col = _w;
	_buffer.ws_row = _h;

	#endif
};
void getConsoleSize(int * _w, int * _h)
{
	//--- Windows code.
	#if defined(_WIN32)

	//--- Get value from the current console buffer.
	CONSOLE_SCREEN_BUFFER_INFO _buffer;
	GetConsoleScreenBufferInfo(GetStdHandle(STD_OUTPUT_HANDLE), &_buffer);
	*_w = _buffer.srWindow.Right - _buffer.srWindow.Left;
	*_h = _buffer.srWindow.Bottom - _buffer.srWindow.Top;

	//--- UNIX code.
	#else

	//--- Get value from the current console buffer.
	struct winsize _buffer;
	ioctl(STDOUT_FILENO, TIOCGWINSZ, &_buffer);
	*_w = _buffer.ws_col;
	*_h = _buffer.ws_row;

	#endif
};

/*--------------------------------------------------------------------------
 OBJECT
--------------------------------------------------------------------------*/
OBJ * objNew(IMG * _IMG)
{
	OBJ * _OBJ_new = (OBJ*)malloc(sizeof(OBJ));
	_OBJ_new->x = 0;
	_OBJ_new->y = 0;
	_OBJ_new->w = 0;
	_OBJ_new->h = 0;
	_OBJ_new->o = 1.f;
	_OBJ_new->IMG = _IMG;
	return _OBJ_new;
};

/*--------------------------------------------------------------------------
 IMAGE
--------------------------------------------------------------------------*/
IMG * imgNew(RGB * _RGB, int _w, int _h)
{
	IMG * _IMG_new = (IMG*)malloc(sizeof(IMG));
	_IMG_new->w = _w; _IMG_new->h = _h;
	_IMG_new->RGB = _RGB;
	return _IMG_new;
};
IMG * imgDuplicate(IMG * _IMG)
{
	IMG * _IMG_new = (IMG*)malloc(sizeof(IMG));
	_IMG_new->w = _IMG->w;
	_IMG_new->h = _IMG->h;
	_IMG_new->RGB = (RGB*)malloc(_IMG->w * _IMG->h * sizeof(RGB));
	memcpy(_IMG_new->RGB, _IMG->RGB, _IMG->w * _IMG->h * sizeof(RGB));
	return _IMG_new;
}
void imgCopy(IMG * _IMG_dst, IMG * _IMG_src)
{
	//--- If the destination is null, create a new structure.
	if (_IMG_src == NULL) { _IMG_dst = NULL; return; }
	if (_IMG_dst == NULL) { _IMG_dst = imgDuplicate(_IMG_src); printf("%p\n", _IMG_dst); return; }

	_IMG_dst->w = _IMG_src->w;
	_IMG_dst->h = _IMG_src->h;
	_IMG_dst->RGB = (RGB*)realloc(_IMG_dst->RGB, _IMG_dst->w * _IMG_dst->h * 10 * sizeof(RGB));
	memcpy(_IMG_dst->RGB, _IMG_src->RGB, _IMG_dst->w * _IMG_dst->h * 10 *  sizeof(RGB));
}

IMG * imgImport(char * _filepath)
{
	//--- Opens file using path from argument in binary read-only mode.
	FILE * _stream = fopen(_filepath, "rb");
	if (_stream == NULL) return NULL;

	//--- Get the file size in bytes.
	fseek(_stream, 0L, SEEK_END);
	size_t _fsize = ftell(_stream);
	fseek(_stream, 0L, SEEK_SET);

	//--- Initialize the data buffer.
	unsigned char * _data = (char*)malloc(_fsize * sizeof(char));
	if (_data == NULL) return NULL;

	//--- Store the file content and close the file stream.
	fread(_data, sizeof(char), _fsize, _stream);
	fclose(_stream);

	//--- Get the first argument of the file. (deeMajikNumbah)
	char * _mode = strtok(_data, "\n\t ");

	//--- Initialize return variables.
	RGB * _RGB; int _w, _h, _d;

	/*//--- ASCII Black and white pixmap.
	//--------------------------------------------------------
	if (!strcmp(_mode, "P1"))
	{
	//--- Loops though the arguments.
	for (int _i = 0; _i < _argc; _i++)
	{
	switch (_i) {
	//--- Assign the first argument to width.
	case 0: _w = (int)strtol(_argw[_i], NULL, 0);
	break;

	//--- Assign the second argument to height and allocate memory for the map.
	case 1: _h = (int)strtol(_argw[_i], NULL, 0);
	_data = (int*)malloc(sizeof(int) * _w * _h);
	break;

	//--- Append the rest of the arguments to the map variable.
	default:;
	int _j = _i - 2; if (_j > _w * _h) break;
	_data[_j] = (int)strtol(_argw[_i], NULL, 0) ? 0xFFFFFF : 0x000000;
	break;
	}
	}
	}

	//--- ASCII Grayscale pixmap.
	//--------------------------------------------------------
	else if (!strcmp(_mode, "P2"))
	{
	int _depth = 0xFF;
	for (int _i = 0; _i < _argc; _i++)
	{
	switch (_i) {
	case 0: _w = (int)strtol(_argw[_i], NULL, 0);
	break;

	case 1: _h = (int)strtol(_argw[_i], NULL, 0);
	_data = (int*)malloc(sizeof(int) * _w * _h);
	break;

	//--- Get and assign the color depth multiplier.
	case 2: _depth = 255 / (int)strtol(_argw[_i], NULL, 0);
	break;

	//--- Convert color to RGB and append it to the map array.
	default:;
	int _j = _i - 3; if (_j > _w * _h) break;
	int _rgb = (int)strtol(_argw[_i], NULL, 0) * _depth;
	_data[_j] = (_rgb << 16) + (_rgb << 8) + (_rgb);
	break;
	}
	}
	}

	//--- ASCII RGB pixmap.
	//--------------------------------------------------------
	else if (!strcmp(_mode, "P3"))
	{
	int _depth;
	for (int _i = 0; _i < _argc; _i++)
	{
	switch (_i) {
	case 0: _w = (int)strtol(_argw[_i], NULL, 0);
	break;

	case 1: _h = (int)strtol(_argw[_i], NULL, 0);
	_data = (int*)malloc(sizeof(int) * _w * _h);
	break;

	case 2: _depth = 0xFF / (int)strtol(_argw[_i], NULL, 0);
	break;

	//--- Convert color to RGB and append it to the map array.
	default:;
	int _j = _i - 3; if (_j > _w * _h * 3) break;

	if (!(_j % 3))
	{
	int _r = (int)strtol(_argw[_i + 0], NULL, 0) * _depth;
	int _g = (int)strtol(_argw[_i + 1], NULL, 0) * _depth;
	int _b = (int)strtol(_argw[_i + 2], NULL, 0) * _depth;
	_data[_j/3] = (_r << 16) + (_g << 8) + (_b);
	}
	break;
	}
	}
	}

	//--- Binary black and white pixmap.
	//--------------------------------------------------------
	else if (!strcmp(_mode, "P4"))
	{
	for (int _i = 0; _i < _argc + 1; _i++)
	{
	switch (_i) {
	case 0: _w = (int)strtol(_argw[_i], NULL, 0);
	break;

	case 1: _h = (int)strtol(_argw[_i], NULL, 0);
	_data = (int*)malloc(sizeof(int) * _w * _h);
	//--- Points the last argument to the data.
	_argw[_i + 1] = strchr(_argw[_i], '\0') + 1;
	break;

	//--- Split the string into bits and append them to the map.
	case 2:
	for (int _j = 0; _j < _w * _h; _j++)
	{
	//--- Get the index of the byte to use.
	int _k = _j / 8;
	//--- Get the index of the bit to use in the byte.
	int _l = 7 - (_j % 8);
	//--- Isolate the selected bit and append it to the map array.
	_data[_j] = (_argw[_i][_k] >> _l & 1) ? 0x000000 : 0xFFFFFF;
	}
	break;
	}
	}
	}*/

	//--------------------------------- Binary Grayscale Image. ---------------------------------
	if (!strcmp(_mode, "P5"))
	{
		//--- Initialize arguments buffer.
		unsigned char ** _argw = (char**)malloc(4U * sizeof(char*));
		if (_argw == NULL) return NULL;

		//--- Register arguments.
		int _n = 0; while (_n < 3)
		{
			_argw[_n] = strtok(NULL, "\n\t ");
			if (_argw[_n] == NULL) break;
			while (_argw[_n][0] == '#')
				_argw[_n] = strtok(NULL, "\n");
			_n++;
		};

		//--- Make sure there is enough argument.
		if (_n < 3) return NULL;

		//--- Get the pointer & size to the data block.
		_argw[3] = strchr(_argw[2], '\0') + 1;
		size_t _dsize = _fsize - (size_t)(_argw[3] - _data);

		//--- Assign each arguments to the corresponding variable.
		_w = atoi(_argw[0]);
		_h = atoi(_argw[1]);
		_d = atoi(_argw[2]);
		_RGB = (RGB*)malloc(_w * _h * sizeof(RGB));

		//--- Store the data in the RGB array.
		for (int _i = 0; _i < _dsize; _i++)
		{
			float _l = (_argw[3][_i] & 0xFF) / (float)_d;
			_RGB[_i] = (RGB) { _l, _l, _l };
		}

		//--- Free the buffer.
		free(_argw);
	}

	//--------------------------------- Binary Chromatic Image. ---------------------------------
	else if (!strcmp(_mode, "P6"))
	{
		//--- Initialize arguments buffer.
		unsigned char ** _argw = (char**)malloc(4U * sizeof(char*));
		if (_argw == NULL) return NULL;

		//--- Register arguments.
		int _n = 0; while (_n < 3)
		{
			_argw[_n] = strtok(NULL, "\n\t ");
			if (_argw[_n] == NULL) break;
			while (_argw[_n][0] == '#')
				_argw[_n] = strtok(NULL, "\n");
			_n++;
		};

		//--- Make sure there is enough argument.
		if (_n < 3) return NULL;

		//--- Get the pointer & size to the data block.
		_argw[3] = strchr(_argw[2], '\0') + 1;
		size_t _dsize = _fsize - (size_t)(_argw[3] - _data);

		//--- Assign each arguments to the corresponding variable.
		_w = atoi(_argw[0]);
		_h = atoi(_argw[1]);
		_d = atoi(_argw[2]);
		_RGB = (RGB*)malloc(_w * _h * sizeof(RGB));

		//--- Store the data in the RGB array.
		for (int _i = 0; _i < _dsize; _i++)
			switch (_i % 3) {
			case 0: _RGB[_i / 3].r = _argw[3][_i] / (float)_d; break;
			case 1: _RGB[_i / 3].g = _argw[3][_i] / (float)_d; break;
			case 2: _RGB[_i / 3].b = _argw[3][_i] / (float)_d; break;
			};

		//--- Free the buffer.
		free(_argw);
	}

	//--------------------------------- Format not recognized. ----------------------------------
	else return NULL;

	//--- Free the buffer.
	free(_data);

	//--- Create the pixmap and return it's pointer.
	return imgNew(_RGB, _w, _h);
};
IMG * imgGenFill(RGB _RGB, int _w, int _h)
{
	IMG * _IMG_new = (IMG*)malloc(sizeof(IMG));
	_IMG_new->w = _w; _IMG_new->h = _h;
	_IMG_new->RGB = (RGB*)malloc(_w * _h * sizeof(RGB));
	for (int _i = 0; _i < _w * _h; _i++) _IMG_new->RGB[_i] = _RGB;
	return _IMG_new;
}
void imgRotate(IMG * _IMG, float _angle)
{
	if (_IMG == NULL) return;
	if (_angle == 0.f) return;

	//--- Convert angle to radian.
	float _pi = 3.141592741f;
	_angle *= _pi / 180.f;

	//--- Image size variables.
	int _w_src = _IMG->w;
	int _h_src = _IMG->h;
	int _w_dst;
	int _h_dst;

	//--- Get the new image width and height.
	if (_angle < -_pi / 2.f || _angle > 0.f && _angle < _pi / 2.f) {
			_w_dst = fabsf(roundf(-_w_src * cosf(_angle) -  _h_src * sinf(_angle)));
			_h_dst = fabsf(roundf(-_w_src * sinf(_angle) + -_h_src * cosf(_angle)));}
	else {	_w_dst = fabsf(roundf(_w_src  * cosf(_angle) -  _h_src * sinf(_angle)));
			_h_dst = fabsf(roundf(-_w_src * sinf(_angle) +  _h_src * cosf(_angle)));}

	//--- Declare and define image arrays.
	RGB * _RGB_src = _IMG->RGB;
	RGB * _RGB_dst = (RGB*)calloc(_w_dst * _h_dst * 2, sizeof(RGB));

	//--- Loop through each pixels of the destination image.
	for (int _i = 0; _i < _w_dst * _h_dst; _i++)
	{
		//--- Get the position of the destination pixel.
		int _x_dst = _i % _w_dst;
		int _y_dst = _i / _w_dst;
		_x_dst -= _w_dst / 2;
		_y_dst -= _h_dst / 2;

		//--- Get the position of the source pixel.
		int _x_src = _x_dst * cosf(-_angle) + _y_dst * sinf(-_angle);
		int _y_src = _x_dst * -sinf(-_angle) + _y_dst * cosf(-_angle);
		_x_src += _w_src / 2; 
		_y_src += _h_src / 2;

		if (_x_src >= 0)
		if (_y_src >= 0)
		if (_x_src < _w_src)
		if (_y_src < _h_src)
		{
			//--- Copy the source pixel to the destination.
			int _j = _x_src + (_y_src * _w_src);
			_RGB_dst[_i] = _RGB_src[_j];
		}
		else
		{
			_RGB_dst[_i] = (RGB){ 0.f, 0.f, 0.f };
		}

		//printf("[%2d,%2d] <- [%2d,%2d]\n", _x_dst, _y_dst, _x_src, _y_src);
	};

	//--- Apply modifications and discard the old image.
	_IMG->RGB = _RGB_dst;
	_IMG->w = _w_dst;
	_IMG->h = _h_dst;
	free(_RGB_src);
}

/*--------------------------------------------------------------------------
 RENDERING
--------------------------------------------------------------------------*/

void scrClear(void)
{
	system("clear||cls");
}
SCR * scrNew()
{
	SCR * _SCR_new = (SCR*)malloc(sizeof(SCR));
	_SCR_new->x = 0;
	_SCR_new->y = 0;
	_SCR_new->w = -1;
	_SCR_new->h = -1;
	_SCR_new->n = 0;
	_SCR_new->OBJ = NULL;
	return _SCR_new;
};
void scrAddObj(SCR * _SCR, OBJ * _OBJ)
{
	_SCR->OBJ = (OBJ**)realloc(_SCR->OBJ, (_SCR->n + 1) * sizeof(OBJ*));
	_SCR->OBJ[_SCR->n] = _OBJ;
	_SCR->n++;
}
void scrRemoveObj(SCR * _SCR, OBJ * _OBJ)
{
	//--- Get index of the object we want to delete.
	int _i = 0; for (; _i <= _SCR->n; _i++)
	{
		if (_i == _SCR->n) return;
		if (_SCR->OBJ[_i] == _OBJ) break;
	};

	//--- Delete the object by overwriting memory adresses.
	memcpy(&_SCR->OBJ[_i], &_SCR->OBJ[_i + 1], _SCR->n - _i);
	_SCR->OBJ = (OBJ**)realloc(_SCR->OBJ, (_SCR->n - 1) * sizeof(OBJ*));

	//--- Decrement the object count.
	_SCR->n--;
}
void scrDelete(SCR * _SCR)
{
	free(_SCR->OBJ);
	_SCR->n = 0;
	_SCR->OBJ = NULL;
}
void scrDraw(SCR * _SCR, RGB _RGB)
{
	if (_SCR == NULL) return;
	//--- UNIX code.
	#ifndef _WIN32

	//--- Escape code to print true color characters.
	printf("\e[48;2;%d;%d;%dm \e[0m", (int)(_RGB.r * 255.f), (int)(_RGB.g * 255.f), (int)(_RGB.b * 255.f));

	//--- Windows code.
	#else

		//--- Initialize the windows palette.
		RGB _RGB_palette[] = {	{ 0.0f, 0.0f, 0.0f }, { 0.0f, 0.0f, 0.5f }, { 0.0f, 0.5f, 0.0f }, { 0.0f, 0.5f, 0.5f },
								{ 0.5f, 0.0f, 0.0f }, { 0.5f, 0.0f, 0.5f }, { 0.5f, 0.5f, 0.0f }, { .75f, .75f, .75f },
								{ 0.5f, 0.5f, 0.5f }, { 0.0f, 0.0f, 1.0f }, { 0.0f, 1.0f, 0.0f }, { 0.0f, 1.0f, 1.0f },
								{ 1.0f, 0.0f, 0.0f }, { 1.0f, 0.0f, 1.0f }, { 1.0f, 1.0f, 0.0f }, { 1.0f, 1.0f, 1.0f }};

		//--- Initialize key variables.
		int _kfg = 0, _kbg = 0, _kch = 0;

		//--- Store the distance between the target and the generated color.
		//--- Init value is the maximum possible value.
		float _dist = 100.f;

		//--- Loop through the palette array.
		for (int _i = 0; _i < 16; _i++)
			for (int _j = 0; _j < 16; _j++)
				for (int _k = 1; _k < 2; _k++)
				{
					//--- Generate a blend of two colors.
					RGB _RGB_temp = rgbBlend(_RGB_palette[_i], _RGB_palette[_j], _k / 4.f);

					//--- Calculate the distance from the target color.
					float _dist_temp = rgbDistance(_RGB, _RGB_temp);

					//--- Select this color if it is closer to the target color than the previous.
					if (_dist_temp < _dist )
					{
						_kfg = _i; 
						_kbg = _j;
						_kch = _k; 
						_dist = _dist_temp;
					};
				};

		//--- Print the colored character.
		SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), _kfg + _kbg * 16);
		putchar(_kch ? 176 : 177); SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), 7);
	#endif

};
void scrRender(SCR * _SCR)
{
	//--- Make sure the scene is valid.
	if (_SCR == NULL) return;
	if (_SCR->OBJ == NULL) return;
	if (_SCR->n <= 0) return;

	//--- Get screen size.
	int _SCR_w, _SCR_h; getConsoleSize(&_SCR_w, &_SCR_h);
	_SCR_w = _SCR->w > 0 ? _SCR->w : _SCR_w;
	_SCR_h = _SCR->h > 0 ? _SCR->h : _SCR_h;

	//--- Loop a code for each pixel of the scene.
	for (int _i = 0; _i < _SCR_w * _SCR_h; _i++)
	{ 
		//--- Get the current pixel position.
		int _x = _i % _SCR_w;
		int _y = _i / _SCR_w;

		//--- Compute the output color by looping through the objects.
		RGB _RGB = { 0.f,0.f,0.f };
		for (int _j = 0; _j < _SCR->n; _j++)
		{
			OBJ * _OBJ = _SCR->OBJ[_j];
			if (_OBJ != NULL)
			if (_OBJ->IMG != NULL)
			if (_OBJ->IMG->w > 0)
			if (_OBJ->IMG->h > 0)
			if (_OBJ->IMG->RGB != NULL)
			{
				//--- Compute the virtual size and position of the object.
				int _OBJ_w = (_OBJ->w) ? _OBJ->w : _OBJ->IMG->w;
				int _OBJ_h = (_OBJ->h) ? _OBJ->h : _OBJ->IMG->h;
				int _OBJ_x = (_SCR_w / 2) - (_OBJ_w / 2) + _OBJ->x - _SCR->x;
				int _OBJ_y = (_SCR_h / 2) - (_OBJ_h / 2) + _OBJ->y - _SCR->y;

				//--- If we are in the object.
				if (_OBJ_x <= _x) 
				if (_OBJ_y <= _y)
				if (_OBJ_x + _OBJ_w > _x)
				if (_OBJ_y + _OBJ_h > _y)
				{
					//--- Get the position of the pixel to draw.
					int _x_local = (_x - _OBJ_x) % _OBJ_w;
					int _y_local = (_y - _OBJ_y) % _OBJ_h;

					//--- Get the index of the pixel to draw.
					int _j = _x_local + (_y_local * _OBJ->IMG->w);

					//--- Blend the color with the background color.
					_RGB = rgbBlend(_OBJ->IMG->RGB[_j], _RGB, _OBJ->o);
				};
			};
		};

		//--- If we need to draw a black pixel, just print a spacer.
		if (!rgbCompare(_RGB, (RGB) { 0.0, 0.0, 0.0 }))
		{
			//--- Set the cursor position and draw the computed color.
			setCursorPos(_x, _y);
			scrDraw(_SCR, _RGB);
		};

	};
};

#endif