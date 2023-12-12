:: // Author: Egor A. Pristavka. 
:: // Copyright 2022-2023 VeVerse AS.
:: // Copyright (c) 2023 LE7EL AS. All right reserved.

:: target directory taken as the first argument
set target_dir=%1

:: create the project intermediate directory
mkdir %target_dir%\Metaverse\Intermediate
:: make link to the project intermediate directory
mklink /D ..\Intermediate %target_dir%\Metaverse\Intermediate

:: create plugin target intermediate directories
for %%s in (VeShared VeSDK VeGame VeInteraction VeOpenSea VeSentry) do mkdir %target_dir%\Metaverse\Plugins\%%s\Intermediate
:: remove local plugin intermediate directories
for %%s in (VeShared VeSDK VeGame VeInteraction VeOpenSea VeSentry) do rmdir ..\Plugins\%%s\Intermediate /s
:: make links for target plugin intermediate directories
for %%s in (VeShared VeSDK VeGame VeInteraction VeOpenSea VeSentry) do mklink /D ..\Plugins\%%s\Intermediate %target_dir%\Metaverse\Plugins\%%s\Intermediate
