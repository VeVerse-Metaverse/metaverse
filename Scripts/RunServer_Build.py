# // Author: Egor A. Pristavka. 
# // Copyright 2022-2023 VeVerse AS.
# // Copyright (c) 2023 LE7EL AS. All right reserved.

# https://docs.unrealengine.com/en-US/ProductionPipelines/CommandLineArguments/
# UnrealInsights: -trace=log,counters,cpu,frame,bookmark,file,loadtime,gpu,rhicommands,net,rendercommands,memory -NetTrace=1

import os
from pathlib import Path
from dataclasses import dataclass

FLAGS = '-log'
ENVIRONMENT = {
    'VE_SERVER_API_KEY': '',
    'VE_SERVER_PUBLIC_PORT': 0000,
    'VE_SERVER_PORT': 0000,
    'VE_SERVER_HOST': '127.0.0.1',
    'VE_SERVER_SPACE_ID': ''
}

DIR = 'artheonui'

TOOL = 'F:\\UE4\\UnrealEngine\\Metaverse\\Saved\\StagedBuilds\\DebugGame\\WindowsServer\\MetaverseServer.exe'

def main():
    # project = get_uproject(DIR)
    command = 'start "" "' + TOOL + '" ' + FLAGS
    # command = '"'+TOOL+'" '+FLAGS

    print(command)
    print('=' * 80)

    global ENVIRONMENT
    for key in ENVIRONMENT:
        os.environ[key] = str(ENVIRONMENT[key])

    os.system(command)


# -------------------------------------------------------------------------------


@dataclass
class UProject:
    path: str
    name: str


def get_uproject(dir: str):
    dir = os.fsdecode(os.path.join(os.getcwdb(), os.fsencode(dir)))
    uprojects = list(Path(dir).glob('*.uproject'))
    return UProject(str(uprojects[0]), uprojects[0].stem)


def pause():
    import msvcrt
    print("")
    print("Press any key to continue...")
    msvcrt.getch()


if __name__ == '__main__':
    main()
    # pause()
