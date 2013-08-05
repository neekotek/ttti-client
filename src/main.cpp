#if defined( _WIN32 ) || defined( _WIN64 )
#include <curses.h>
#elif defined( __linux__ )
#include <curses.h>
#endif

extern void __sleep( unsigned int ms );

const unsigned int KEY_NUM7 = 449;
const unsigned int KEY_NUM8 = 450;
const unsigned int KEY_NUM9 = 451;
const unsigned int KEY_NUM4 = 452;
const unsigned int KEY_NUM5 = 453;
const unsigned int KEY_NUM6 = 454;
const unsigned int KEY_NUM1 = 455;
const unsigned int KEY_NUM2 = 456;
const unsigned int KEY_NUM3 = 457;

const unsigned int ESC_KEY = 27;
const unsigned int EXIT_KEY = 17; // ^Q
const unsigned int ENTER_KEY = 10;

bool isMoveKey( int key )
{
	if( ( key >= 449 && key <= 457 ) || ( key >= 0x102 && key <= 0x105 ) )
		return true;
	return false;
}

void initColors()
{
	init_pair( 1, COLOR_GREEN, COLOR_BLACK );
	init_pair( 2, COLOR_RED, COLOR_BLACK );
	init_pair( 3, COLOR_BLUE, COLOR_BLACK );
	init_pair( 4, COLOR_WHITE, COLOR_BLACK );
	init_pair( 5, COLOR_CYAN, COLOR_BLACK );
	init_pair( 6, COLOR_YELLOW, COLOR_BLACK );
	init_pair( 7, COLOR_MAGENTA, COLOR_BLACK );
	init_pair( 8, COLOR_RED, COLOR_RED );
	init_pair( 9, COLOR_BLUE, COLOR_BLUE );
	init_pair( 10, COLOR_BLUE, COLOR_RED );
	init_pair( 11, COLOR_RED, COLOR_BLUE );
	init_pair( 12, COLOR_RED, COLOR_RED );
	init_pair( 13, COLOR_WHITE, COLOR_BLUE );
	init_pair( 14, COLOR_WHITE, COLOR_RED );
}

#define COLOR_OGREEN COLOR_PAIR( 1 )
#define COLOR_GREENB COLOR_OGREEN | A_BOLD
#define COLOR_ORED COLOR_PAIR( 2 )
#define COLOR_REDB COLOR_ORED | A_BOLD
#define COLOR_OBLUE COLOR_PAIR( 3 )
#define COLOR_BLUEB COLOR_OBLUE | A_BOLD
#define COLOR_OWHITE COLOR_PAIR( 4 )
#define COLOR_WHITEB COLOR_OWHITE | A_BOLD
#define COLOR_GRAY COLOR_OWHITE | A_DIM
#define COLOR_OCYAN COLOR_PAIR( 5 )
#define COLOR_CYANB COLOR_OCYAN | A_BOLD
#define COLOR_OYELLOW COLOR_PAIR( 6 )
#define COLOR_YELLWB COLOR_OYELLOW | A_BOLD
#define COLOR_OMAGENTA COLOR_PAIR( 7 )
#define COLOR_MAGENTAB COLOR_OMAGENTA | A_BOLD

#define COLOR_BORDER COLOR_GRAY
#define COLOR_UNOCCUPIED COLOR_WHITEB
#define COLOR_UNOCCUPIED_NWON COLOR_PAIR( 13 ) | A_BOLD
#define COLOR_UNOCCUPIED_CWON COLOR_PAIR( 14 )
#define COLOR_NAUGHT COLOR_BLUEB
#define COLOR_NAUGHT_NWON COLOR_PAIR( 9 ) | A_BOLD
#define COLOR_NAUGHT_CWON COLOR_PAIR( 10 ) | A_BOLD
#define COLOR_CROSS COLOR_REDB
#define COLOR_CROSS_NWON COLOR_PAIR( 11 )
#define COLOR_CROSS_CWON COLOR_PAIR( 12 ) | A_BOLD

void setTitle( const char * title )
{
#if defined( __PDCURSES__ )
	PDC_set_title( title );
#endif
}

const unsigned int MAP_X = 13;
const unsigned int MAP_Y = 13;

enum tileState
{
	border_tile,
	unoccupied,
	unoccupied_nwon,
	unoccupied_cwon,
	naught,
	naught_nwon,
	naught_cwon,
	cross,
	cross_nwon,
	cross_cwon
} map[ MAP_X ][ MAP_Y ];

int screen[ MAP_X ][ MAP_Y ];

void initializeMap()
{
	for( unsigned int x = 0; x < MAP_X; x++ )
		for( unsigned int y = 0; y < MAP_Y; y++ )
		{
			map[ x ][ y ] = ( x % 4 == 0 || y % 4 == 0 ) ? border_tile : unoccupied;
		}
}

int curFieldX = 1;
int curFieldY = 1;
int curPosX = 1;
int curPosY = 1;

void draw()
{
	for( unsigned int x = 0; x < MAP_X; x++ )
		for( unsigned int y = 0; y < MAP_Y; y++ )
		{
			unsigned int icon;
			switch( map[ x ][ y ] )
			{
			case border_tile:
				if( x % 4 == 0 )
					icon = '|';
				else if( y % 4 == 0 )
					icon = '-';
				if( x % 4 == 0 && y % 4 == 0 )
					icon = '+';
				icon |= COLOR_BORDER;
				break;
			case unoccupied:
				icon = '.' | COLOR_UNOCCUPIED;
				break;
			case unoccupied_nwon:
				icon = '.' | COLOR_UNOCCUPIED_NWON;
				break;
			case unoccupied_cwon:
				icon = '.' | COLOR_UNOCCUPIED_CWON;
				break;
			case naught:
				icon = 'O' | COLOR_NAUGHT;
				break;
			case naught_nwon:
				icon = 'O' | COLOR_NAUGHT_NWON;
				break;
			case naught_cwon:
				icon = 'O' | COLOR_NAUGHT_CWON;
				break;
			case cross:
				icon = 'X' | COLOR_CROSS;
				break;
			case cross_nwon:
				icon = 'X' | COLOR_CROSS_NWON;
				break;
			case cross_cwon:
				icon = 'X' | COLOR_CROSS_CWON;
				break;
			}
			//screen[ x ][ y ] = icon;
			mvaddch( y, x, icon );
		}
		redrawwin( stdscr );
		move( curPosY, curPosX );
		wrefresh( stdscr );
}

enum
{
	moveField,
	moveGlobal
} gameState = moveGlobal;

enum playerType
{
	crosses,
	naughts
} player = crosses;

enum direction
{
	none,
	north,
	northwest,
	west,
	southwest,
	south,
	southeast,
	east,
	northeast
};

bool makeMove( int action )
{
	int offsetX = 0;
	int offsetY = 0;
	
		switch( action )
		{
		// add ctrl+z, enter, keys to switch tile type, etc
		case KEY_UP:
		case KEY_NUM8:
			--offsetY;
			break;
		case KEY_NUM9:
			++offsetX;
			--offsetY;
			break;
		case KEY_RIGHT:
		case KEY_NUM6:
			++offsetX;
			break;
		case KEY_NUM3:
			++offsetX;
			++offsetY;
			break;
		case KEY_DOWN:
		case KEY_NUM2:
			++offsetY;
			break;
		case KEY_NUM1:
			--offsetX;
			++offsetY;
			break;
		case KEY_LEFT:
		case KEY_NUM4:
			--offsetX;
			break;
		case KEY_NUM7:
			--offsetX;
			--offsetY;
			break;
		}
		if( gameState != moveGlobal &&  map[ curPosX + offsetX ][ curPosY + offsetY ] == border_tile
			|| curPosX + offsetX == 0 || curPosX + offsetX == MAP_X - 1
			|| curPosY + offsetY == 0 || curPosY + offsetY == MAP_Y - 1 )
			return false;
		curPosX += offsetX;
		curPosY += offsetY;
		return true;
}

bool isFree( tileState state )
{
	if( state == unoccupied || state == unoccupied_nwon || state == unoccupied_cwon )
		return true;
	return false;
}

bool isWon( tileState state )
{
	if( state == unoccupied_nwon || state == unoccupied_cwon || state == naught_nwon || state == naught_cwon
		|| state == cross_nwon || state == cross_cwon )
		return true;
	return false;
}

bool placeMark()
{
	/*if( gameState == moveGlobal && map[ curPosX ][ curPosY ] == border_tile )
		return false;
	if( !isFree( map[ curPosX ][ curPosY ] ) )
	{
		bool freeChoice = true;
		for( unsigned int x = curFieldX + ( curFieldX - 1 ) * 3; x < curFieldX + curFieldX * 3 && freeChoice; x++ )
			for( unsigned int y = curFieldY + ( curFieldY - 1 ) * 3; y < curFieldY + curFieldY * 3 && freeChoice; y++ )
			{
				if( isFree( map[ x ][ y ] ) )
					freeChoice = false;
			}
		if( freeChoice )
			gameState = moveGlobal;
		return false;
	}*/
	if( !isFree( map[ curPosX ][ curPosY ] ) )
		return false;
	curFieldX = curPosX / 4 + 1;
	curFieldY = curPosY / 4 + 1;
	tileState markType;
	switch( map[ curPosX ][ curPosY ] )
	{
	case unoccupied:
		if( player == crosses )
			markType = cross;
		else
			markType = naught;
		break;
	case unoccupied_nwon:
		if( player == crosses )
			markType = cross_nwon;
		else
			markType = naught_nwon;
		break;
	case unoccupied_cwon:
		if( player == crosses )
			markType = cross_cwon;
		else
			markType = naught_cwon;
		break;
	}
	map[ curPosX ][ curPosY ] = markType;
	// check the victory conditions
	if( !isWon( map[ curPosX ][ curPosY ] ) )
	{
		bool won = false;
		for( unsigned int x = curFieldX + ( curFieldX - 1 ) * 3, y = curFieldY + ( curFieldY - 1 ) * 3; x < curFieldX + curFieldX * 3; x++ )
		{
			markType = player == crosses ? cross : naught;
			if( map[ x ][ y ] == markType && map[ x ][ y + 1 ] == markType && map[ x ][ y + 2 ] == markType )
			{
				won = true;
				break;
			}
		}
		for( unsigned int x = curFieldX + ( curFieldX - 1 ) * 3, y = curFieldY + ( curFieldY - 1 ) * 3; y < curFieldY + curFieldY * 3 && !won; y++ )
		{
			if( map[ x ][ y ] == markType && map[ x + 1 ][ y ] == markType && map[ x + 2 ][ y ] == markType )
				won = true;
		}
		if( !won )
		{
			unsigned int x = curFieldX + ( curFieldX - 1 ) * 3;
			unsigned int y = curFieldY + ( curFieldY - 1 ) * 3;
			if( map[ x + 1 ][ y + 1 ] == markType ) // central tile
			{
				if( ( map[ x ][ y  ] == markType && map[ x + 2 ][ y + 2 ] == markType ) || ( map[ x + 2 ][ y ] == markType && map[ x ][ y + 2 ] == markType ) )
					won = true;
			}
		}
		if( won )
		{
			for( unsigned int x = curFieldX + ( curFieldX - 1 ) * 3; x < curFieldX + curFieldX * 3; x++ )
				for( unsigned int y = curFieldY + ( curFieldY - 1 ) * 3; y < curFieldY + curFieldY * 3; y++ )
				{
					switch( map[ x ][ y ] )
					{
					case unoccupied:
						if( markType == cross )
							map[ x ][ y ] = unoccupied_cwon;
						else
							map[ x ][ y ] = unoccupied_nwon;
						break;
					case cross:
						if( markType == cross )
							map[ x ][ y ] = cross_cwon;
						else
							map[ x ][ y ] = cross_nwon;
						break;
					case naught:
						if( markType == cross )
							map[ x ][ y ] = naught_cwon;
						else
							map[ x ][ y ] = naught_nwon;
						break;
					}
				}
		}
	}
	// move the player to the appropriate field, check if it's full
	curFieldX = curPosX % 4;
	curFieldY = curPosY % 4;
	curPosX = curFieldX + ( curFieldX - 1 ) * 4;
	curPosY = curFieldY + ( curFieldY - 1 ) * 4;
	gameState = moveField;
	bool freeSpace = false;
	for( unsigned int x = curFieldX + ( curFieldX - 1 ) * 3; x < curFieldX + curFieldX * 3 && !freeSpace; x++ )
		for( unsigned int y = curFieldY + ( curFieldY - 1 ) * 3; y < curFieldY + curFieldY * 3 && !freeSpace; y++ )
		{
			if( isFree( map[ x ][ y ] ) )
				freeSpace = true;
		}
	if( !freeSpace )
		gameState = moveGlobal;
	return true;
}

int main( int argc, char * argv[] )
{
	initscr();
	//refresh(); // fixed some shit in DoO, putting it here just to be sure
	setTitle( "Dungeons of Ololollo" );
	start_color();
	initColors(); // initialize color pairs
	noecho();
	cbreak();//nodelay( stdscr, TRUE );
	keypad( stdscr, TRUE );

	initializeMap();
	draw();
	int action;
	while( ( action = getch() ) != EXIT_KEY )
	{
		if( isMoveKey( action ) )
			makeMove( action );
		else if( action == ENTER_KEY )
		{
			if(	placeMark() )
			{
				if( player == crosses )
					player = naughts;
				else
					player = crosses;
			}
		}
		draw();
		//sleep( 10 );
	}
	endwin();
	return 0;
}
