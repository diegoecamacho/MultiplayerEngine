@echo off
START %~dp0\Game3110_Engine\Debug\Game3110_GameServer.exe
echo Server Started!

TIMEOUT /T 2
echo Client Started!
START %~dp0\Game3110_Engine\Debug\Game3110_Game.exe
START %~dp0\Game3110_Engine\Debug\Game3110_Game.exe
START %~dp0\Game3110_Engine\Debug\Game3110_Game.exe
pause