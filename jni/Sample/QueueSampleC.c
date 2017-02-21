#define  UNICODE
#define _UNICODE
#define WIN32_LEAN_AND_MEAN  // Exclude rarely-used 
#include <windows.h>

__declspec( align( 64 )) static volatile LONG _Counter;
__declspec( align( 64 )) static          LONG _InternC;
__declspec( align( 64 )) static volatile LONG _CheckIt;

#if 1
#define LOG(n) Log n
//-------------------------------------------------------------------------------------------------
static VOID Log( char *msg, ... ) {
//-------------------------------------------------------------------------------------------------

    CHAR buf[ 128 ];
    va_list vaList ;

    va_start( vaList, msg );
    wvsprintfA( buf, msg, vaList );
    va_end( vaList );

    OutputDebugStringA( buf );
}
#else
#define LOG(n)
#endif

//=================================================================================================
DWORD WINAPI pfnThread( LPVOID lpParam ) {
//=================================================================================================

    UNREFERENCED_PARAMETER( lpParam );

    LOG(( "QUEUE-T: Started\n" ));

    for (;;) {

        if ( SleepEx( 10, TRUE ) == WAIT_IO_COMPLETION ) {
            ;
        } else if ( _InterlockedCompareExchange( &_Counter, -1, _InternC ) != _InternC ) {
            LOG(( "QUEUE-T: Queue not empty. %d/%d\n", _InternC, _Counter ));
        } else {
            LOG(( "QUEUE-T: Timeout\n" ));
            break;
        } /* endif */

    } /* endfor */

    LOG(( "QUEUE-T: Exit\n" ));

    return 0;
}

//=================================================================================================
VOID CALLBACK pfnApc( ULONG_PTR dwParam ) {
//=================================================================================================

    UNREFERENCED_PARAMETER( dwParam );

    _InternC++;

    LOG(( "QUEUE-A: %d\n", _InternC ));

    _InterlockedDecrement( &_CheckIt );
}

//=================================================================================================
VOID rawmain( VOID ) {
//=================================================================================================

    DWORD dwThreadId, i, j, c = 0;
    HANDLE hThread;

    _CheckIt = 0;

    LOG(( "QUEUE-M: Started\n" ));

    for ( j = 0; j < 30; j++ ) {

        _Counter = 0;
        _InternC = 0; // do not init in pfnThread. Apc runs in advance!

        LOG(( "QUEUE-M: New thread\n" ));

        hThread = CreateThread( NULL, 0, pfnThread, (LPVOID)NULL, 0, &dwThreadId );

        for ( i = 0; i < 40; i++ ) {

            if (( c = _InterlockedIncrement( &_Counter )) == 0L ) break;
            QueueUserAPC( pfnApc, hThread, 0 );

            _InterlockedIncrement( &_CheckIt );

            Sleep( 10 );

        } /* endfor */

        if ( c == 0 ) {
            LOG(( "QUEUE-M: Thread will quit\n" ));
        } else {
            LOG(( "QUEUE-M: All queued. %d\n", c ));
        } /* endif */

        WaitForSingleObject( hThread, INFINITE );

        LOG(( "QUEUE-M: Thread gone\n" ));

    } /* endfor */

    LOG(( "QUEUE-M: Exit. Lost=%d\n", _CheckIt ));

    ExitProcess( _CheckIt ? 1 : 0 );
}

// -=EOF=-
