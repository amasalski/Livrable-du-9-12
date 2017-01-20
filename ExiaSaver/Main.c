#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>
#include <time.h>
#if defined(_WIN32)
	#include <windows.h>
	#include "dirent.h"
	#define sleep(x) Sleep(x * 0.1)
#else
	#include <unistd.h>
	#include <dirent.h>
	#include <pthread.h>
	#include <sys/ioctl.h>
	#define sleep(x) usleep(x * 100)
#endif
#ifndef _MSC_VER
#define max(a,b) (((a) > (b)) ? (a) : (b))
#define min(a,b) (((a) < (b)) ? (a) : (b))
#endif
#include "Color.h"
#include "Engine.h"

void scrRenderRealtime(SCR * _SCR)
{
	//--- Clear the screen
	scrClear();
	atexit(scrClear);
	//--- Loop the render functions.
	while (_SCR != NULL) scrRender(_SCR);
};

#ifndef _WIN32
int getch(void)
{
	struct termios oldattr, newattr;
	int ch;
	tcgetattr(STDIN_FILENO, &oldattr);
	newattr = oldattr;
	newattr.c_lflag &= ~(ICANON | ECHO);
	tcsetattr(STDIN_FILENO, TCSANOW, &newattr);
	ch = getchar();
	tcsetattr(STDIN_FILENO, TCSANOW, &oldattr);
	return ch;
}
#endif

void registerInput(int * _buffer)
{
	while(1)
	{
		*_buffer = getch();
		sleep(1000);
		*_buffer = 0;
	}
}

int listFile(char ** _filelist, const char * _path, const char * _ext)
{
	if (_path == NULL) return 0;
	printf("%p", _filelist);

	//--- Opens and start reading the directory.
	DIR * _DIR = opendir(_path);
	if (_DIR == NULL) return 0;
	struct dirent * _stream = readdir(_DIR);


	//--- Read all files from the selected directory
	int _i = 0; while( _stream = readdir(_DIR) )
	{
		//--- Get the file extension.
		char * _ext_cur = strrchr(_stream->d_name, '.');

		//--- Verify the file extension.
		if (_ext_cur != NULL)
			if (!strcmp(_ext, _ext_cur))
			{
				//--- Allocate variable to store the file path.
				char * _filepath = (char*)malloc((strlen(_path) + strlen(_stream->d_name)) * sizeof(char));

				//--- Format the path of the file.
				sprintf(_filepath, "%s/%s", _path, _stream->d_name);
				printf("%s/%s\n", _path, _stream->d_name);

				//--- Append the path to the path list
				if (_filelist != NULL)
					_filelist[_i] = _filepath;
				_i++;
			};
	};

	closedir(_DIR);
	return _i;
}


void writeStat(const char * _directory, int _playmode)
{
	char _filepath[4096];

	//--- Get log file path.
	#if defined(_WIN32)
	GetCurrentDirectoryA(4096, _filepath);
	#else
	getcwd(_filepath, 4096);
	#endif
	sprintf(_filepath, "%s/exiasaver.log", _filepath);

	//--- Open file
	FILE * _stream = fopen(_filepath, "a");
	if (_stream == NULL) return;

	//--- Initialize output 
	char _output[4096];

	//--- Append the time to the output string.
	time_t mytime = time(NULL); strftime(_output, 25, "%c", localtime(&mytime));

	//--- Append the playmode & the ressource directory.
	sprintf(_output, "%s - %d", _output, _playmode);
	sprintf(_output,"%s - '%s'\n", _output, _directory);

	//--- Append the output to the log file.
	fputs(_output, _stream);
	fflush(_stream);

	//--- Close.
	fclose(_stream);
};
void printStat(void)
{
	//--- Get log file path.
	char * _filepath[4096];
	#if defined(_WIN32)
	GetCurrentDirectoryA(4096, _filepath);
	#else
	getcwd(_filepath, 4096);
	#endif
	sprintf(_filepath, "%s/exiasaver.log", _filepath);

	//--- Open file
	FILE * _stream = fopen(_filepath, "rb");
	if (_stream == NULL) return;

	//--- Get the file size in bytes.
	fseek(_stream, 0L, SEEK_END);
	size_t _fsize = ftell(_stream);
	fseek(_stream, 0L, SEEK_SET);

	printf("Show history (Y/N): ");
	char _showhistory;
	scanf("%c", &_showhistory);
	system("clear||cls");

	//--- Initialize the data buffer.
	if (_showhistory == 'Y')
	{
		printf("\n------- HISTORY\n\n");
		char * _log = (char*)malloc(_fsize * sizeof(char));
		if (_log == NULL) return;
		fread(_log, sizeof(char), _fsize, _stream);
		printf("%s", _log); free(_log);
		fseek(_stream, 0L, SEEK_SET); 
		printf("\n");
	};

	printf("------- STATISTICS\n\n");

	//--- Initialise variables.
	char	_line[4096];
	int		_playCount[16];

	//--- Force set zero.
	for (int _i = 0; _i < 15; _i++)
		_playCount[_i] = 0;

	while (!feof(_stream))
	{
		//--- Get the line string.
		fgets(_line, 4096, _stream);

		//--- Isolate the playmode name
		char * _playmode = strchr(_line, '-') + 2;
		strtok(_playmode, " ");

		int _id = atoi(_playmode);
		if (_id < 16)
			_playCount[_id]++;
	};

	//--- Compute sum.
	int _sum = 0; for (int _i = 0; _i < 15; _i++)
		_sum += _playCount[_i];

	//--- For each playmode
	for (int _i = 0; _i < 15; _i++)
	{
		//--- Find the maximum playmode index
		int _imax = 0; for (int _j = 0; _j < 15; _j++)
			if (_playCount[_j] > _playCount[_imax]) _imax = _j;

		//--- If playmode has been played more than once.
		if (_playCount[_imax] > 0)
		{
			char _playName[128];
			switch (_imax)
			{
				case 0: strcpy(_playName, "playDiapo - Static:\t"); break;
				case 1: strcpy(_playName, "playClock - Dynamic:\t"); break;
				case 2: strcpy(_playName, "playTopgn - Interctive:\t"); break;
			};

			printf("%s %2d - %2d%%\n", _playName, _playCount[_imax], 100 * _playCount[_imax] / _sum);
		};

		//--- Avoid printing the same mode again.
		_playCount[_imax] = 0;
	};

	fclose(_stream);
	fseek(stdin, 0, SEEK_END);
	getchar();
	exit(0);
};

#include "PlayClock.h"
#include "PlayDiapo.h"
//#include "PlayTopgun.h"

void playDebug(_filepath)
{
	if (_filepath == NULL) return;

	//--- Initialize the image list.
	int _n = listFile(NULL, _filepath, ".pnm"); if (_n == 0) return;
	char ** _filelist = (char**)malloc(_n * sizeof(char*));
	IMG ** _IMG_list = (IMG**)malloc(_n * sizeof(IMG*));

	//--- Fill the image list.
	listFile(_filelist, _filepath, ".pnm");
	for (int _i = 0; _i < _n; _i++)
		_IMG_list[_i] = imgImport(_filelist[_i]);

	//--- Select a new image and push the last one in the background.
	IMG * _IMG_cur = _IMG_list[rand() % _n];
	IMG * _IMG_temp = imgNew(NULL, 0, 0);


	//--- Initialize the screen and objects.
	SCR * _SCR = scrNew();
	scrAddObj(_SCR, objNew(_IMG_temp));

	//--- Loop the rendering function in a thread.
	#if defined(_WIN32)
	CreateThread(NULL, 0, (LPTHREAD_START_ROUTINE)scrRenderRealtime, _SCR, 0, NULL);
	#else
	pthread_t _thread;
	pthread_create(&_thread, NULL, scrRenderRealtime, _SCR);
	#endif

	float p;
	while (_SCR != NULL)
	{
		imgCopy(_IMG_temp, _IMG_cur);
		imgRotate(_IMG_temp, p+=90.f);
		sleep(50000);
	}
}

int main(int _argc, char ** _argv)
{
	//--- Generate seed.
	srand(time(NULL));
	//--- Hide console cursor.
	showCursor(0);
	
	//--- Declare variables.
	int		_G_playmode = -1;
	char *	_G_directory = (char*)calloc(4096, sizeof(char));

	//--- Set default directory
	#if defined(_WIN32)
	GetCurrentDirectoryA(4096, _G_directory);
	#else
	getcwd(_G_directory, 4096);
	#endif
	sprintf(_G_directory, "%s/data", _G_directory);

	//--- If the environment variable is set. apply it.
	if (getenv("EXIASAVER_HOME")) _G_directory = getenv("EXIASAVER_HOME");

	//--- Read parameters and play
	for (int _i = 0; _i < _argc; _i++)
	{
		if		(!strcmp(_argv[_i], "-stats"))			printStat();
		else if	(!strcmp(_argv[_i], "-diapo"))			_G_playmode = 0;
		else if (!strcmp(_argv[_i], "-clock"))			_G_playmode = 1;
		else if (!strcmp(_argv[_i], "-topgn"))			_G_playmode = 2;
		else if (strstr(_argv[_i], "-dir=") != NULL)	_G_directory = strchr(_argv[_i],'=') + 1;
	};

	//--- If the mode is not set, randomly set it.
	if (_G_playmode < 0) _G_playmode = rand() % 3;

	//--- Debug
	writeStat(_G_directory, _G_playmode);

	//--- Launch according to the playmode.
	switch (_G_playmode)
	{
		case 0: playDiapo(_G_directory); break;
		case 1: playClock(_G_directory); break;
		case 2: playClock(_G_directory); break;
	}

	return 0;
}