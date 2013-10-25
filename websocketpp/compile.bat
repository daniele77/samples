REM SET websocketpp_path=..\..\..\websocket\websocketpp-master
SET websocketpp_path=E:\progetti\websocketpp\websocketpp-master\

cl /EHa /DWIN32 /I%websocketpp_path% /I%BOOST% echo_server.cpp /link /LIBPATH:%BOOST%\stage\lib
