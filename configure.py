#!/usr/bin/env python3

###
# Generates build files for the project.
# This file also includes the project configuration,
# such as compiler flags and the object matching status.
#
# Usage:
#   python3 configure.py
#   ninja
#
# Append --help to see available options.
###

import argparse
import sys
from pathlib import Path
from typing import Any, Dict, List

from tools.project import (
    Object,
    ProgressCategory,
    ProjectConfig,
    calculate_progress,
    generate_build,
    is_windows,
)

# Game versions
DEFAULT_VERSION = 0
VERSIONS = [
    "HOTWHE",  # 0
]

parser = argparse.ArgumentParser()
parser.add_argument(
    "mode",
    choices=["configure", "progress"],
    default="configure",
    help="script mode (default: configure)",
    nargs="?",
)
parser.add_argument(
    "-v",
    "--version",
    choices=VERSIONS,
    type=str.upper,
    default=VERSIONS[DEFAULT_VERSION],
    help="version to build",
)
parser.add_argument(
    "--build-dir",
    metavar="DIR",
    type=Path,
    default=Path("build"),
    help="base build directory (default: build)",
)
parser.add_argument(
    "--binutils",
    metavar="BINARY",
    type=Path,
    help="path to binutils (optional)",
)
parser.add_argument(
    "--compilers",
    metavar="DIR",
    type=Path,
    help="path to compilers (optional)",
)
parser.add_argument(
    "--map",
    action="store_true",
    help="generate map file(s)",
)
parser.add_argument(
    "--debug",
    action="store_true",
    help="build with debug info (non-matching)",
)
if not is_windows():
    parser.add_argument(
        "--wrapper",
        metavar="BINARY",
        type=Path,
        help="path to wibo or wine (optional)",
    )
parser.add_argument(
    "--dtk",
    metavar="BINARY | DIR",
    type=Path,
    help="path to decomp-toolkit binary or source (optional)",
)
parser.add_argument(
    "--objdiff",
    metavar="BINARY | DIR",
    type=Path,
    help="path to objdiff-cli binary or source (optional)",
)
parser.add_argument(
    "--sjiswrap",
    metavar="EXE",
    type=Path,
    help="path to sjiswrap.exe (optional)",
)
parser.add_argument(
    "--ninja",
    metavar="BINARY",
    type=Path,
    help="path to ninja binary (optional)",
)
parser.add_argument(
    "--verbose",
    action="store_true",
    help="print verbose output",
)
parser.add_argument(
    "--non-matching",
    dest="non_matching",
    action="store_true",
    help="builds equivalent (but non-matching) or modded objects",
)
parser.add_argument(
    "--warn",
    dest="warn",
    type=str,
    choices=["all", "off", "error"],
    help="how to handle warnings",
)
parser.add_argument(
    "--no-progress",
    dest="progress",
    action="store_false",
    help="disable progress calculation",
)
args = parser.parse_args()

config = ProjectConfig()
config.version = str(args.version)
version_num = VERSIONS.index(config.version)

# Apply arguments
config.build_dir = args.build_dir
config.dtk_path = args.dtk
config.objdiff_path = args.objdiff
config.binutils_path = args.binutils
config.compilers_path = args.compilers
config.generate_map = args.map
config.non_matching = args.non_matching
config.sjiswrap_path = args.sjiswrap
config.ninja_path = args.ninja
config.progress = args.progress
if not is_windows():
    config.wrapper = args.wrapper
# Don't build asm unless we're --non-matching
if not config.non_matching:
    config.asm_dir = None

# Tool versions
config.binutils_tag = "2.42-2"
config.compilers_tag = "20251118"
config.dtk_tag = "v1.8.3"
config.objdiff_tag = "v3.6.1"
config.sjiswrap_tag = "v1.2.2"
config.wibo_tag = "1.0.3"

# Project
config.config_path = Path("config") / config.version / "config.yml"
config.check_sha_path = Path("config") / config.version / "build.sha1"
config.asflags = [
    "-mgekko",
    "--strip-local-absolute",
    "-I include",
    f"-I build/{config.version}/include",
    f"--defsym BUILD_VERSION={version_num}",
]
config.ldflags = [
    "-fp hardware",
    "-nodefaults",
]
if args.debug:
    config.ldflags.append("-g")  # Or -gdwarf-2 for Wii linkers
if args.map:
    config.ldflags.append("-mapunused")
    # config.ldflags.append("-listclosure") # For Wii linkers

# Use for any additional files that should cause a re-configure when modified
config.reconfig_deps = []

# Optional numeric ID for decomp.me preset
# Can be overridden in libraries or objects
config.scratch_preset_id = None

# Base flags, common to most GC/Wii games.
# Generally leave untouched, with overrides added below.
cflags_base = [
    "-nodefaults",
    "-proc gekko",
    "-align powerpc",
    "-enum int",
    "-fp hardware",
    "-Cpp_exceptions off",
    "-O4,p",
    "-inline auto",
    '-pragma "cats off"',
    '-pragma "warn_notinlined off"',
    "-maxerrors 1",
    "-nosyspath",
    "-RTTI off",
    "-fp_contract on",
    "-str reuse",
    "-multibyte",  # For Wii compilers, replace with `-enc SJIS`
    "-i include",
    f"-i build/{config.version}/include",
    f"-DBUILD_VERSION={version_num}",
    f"-DVERSION_{config.version}",
]

# Debug flags
if args.debug:
    # Or -sym dwarf-2 for Wii compilers
    cflags_base.extend(["-sym on", "-DDEBUG=1"])
else:
    cflags_base.append("-DNDEBUG=1")

# Warning flags
if args.warn == "all":
    cflags_base.append("-W all")
elif args.warn == "off":
    cflags_base.append("-W off")
elif args.warn == "error":
    cflags_base.append("-W error")

# Metrowerks library flags
cflags_runtime = [
    *cflags_base,
    "-use_lmw_stmw on",
    "-str reuse,pool,readonly",
    "-gccinc",
    "-common off",
    "-inline auto",
]

# REL flags
cflags_rel = [
    *cflags_base,
    "-sdata 0",
    "-sdata2 0",
]

config.linker_version = "GC/1.3.2"
config.shift_jis = False


# Helper function for Dolphin libraries
def DolphinLib(lib_name: str, objects: List[Object]) -> Dict[str, Any]:
    return {
        "lib": lib_name,
        "mw_version": "GC/1.2.5n",
        "cflags": cflags_base,
        "progress_category": "sdk",
        "objects": objects,
    }


# Helper function for REL script objects
def Rel(lib_name: str, objects: List[Object]) -> Dict[str, Any]:
    return {
        "lib": lib_name,
        "mw_version": "GC/1.3.2",
        "cflags": cflags_rel,
        "progress_category": "game",
        "objects": objects,
    }


Matching = True                   # Object matches and should be linked
NonMatching = False               # Object does not match and should not be linked
Equivalent = config.non_matching  # Object should be linked when configured with --non-matching


# Object is only matching for specific versions
def MatchingFor(*versions):
    return config.version in versions


config.warn_missing_config = True
config.warn_missing_source = False
config.libs = [
    {
        "lib": "game",
        "mw_version": config.linker_version,
        "cflags": cflags_base,
        "progress_category": "game",
        "objects": [
            Object(NonMatching, "Action.c"),
            Object(NonMatching, "AI.c"),
            Object(NonMatching, "AI_CarA.c"),
            Object(NonMatching, "AnimHandler.c"),
            Object(Matching, "AttachThing.c"),
            Object(NonMatching, "AudioManager.c"),
            Object(Matching, "BSPN0.c"),
            Object(NonMatching, "CameraManager.c"),
            Object(NonMatching, "CarA.c"),
            Object(NonMatching, "CarA_Stunts.c"),
            Object(NonMatching, "CarA_Suspension.c"),
            Object(Matching, "TimeHandler.c"),
            Object(NonMatching, "CollisionPoly.c"),
            Object(NonMatching, "CollisionTerrain.c"),
            Object(NonMatching, "CollisionThing.c"),
            Object(Matching, "CollisionTriangle.c"),
            Object(Matching, "Control.c"),
            Object(NonMatching, "CoronaHolder.c"),
            Object(NonMatching, "Creature.c"),
            Object(NonMatching, "DataGamePackLoad.c"),
            Object(Matching, "Debris.c"),
            Object(NonMatching, "Destructible.c"),
            Object(NonMatching, "Emitter.c"),
            Object(NonMatching, "EventSequence.c"),
            Object(Matching, "ExtractKeys.c"),
            Object(NonMatching, "FileStreaming.c"),
            Object(Matching, "ForceFeedback.c"),
            Object(NonMatching, "GameSettings.c"),
            Object(Matching, "GeometryHandler.c"),
            Object(NonMatching, "GridManager.c"),
            Object(NonMatching, "InterfaceManager.c"),
            Object(Matching, "ItemThing.c"),
            Object(Matching, "LensFlare.c"),
            Object(Matching, "LightHolder.c"),
            Object(Matching, "LineManager.c"),
            Object(Matching, "LogHandler.c"),
            Object(Matching, "Looker.c"),
            Object(NonMatching, "Main.c"),
            Object(NonMatching, "MatrixCache.c"),
            Object(NonMatching, "NavHolder.c"),
            Object(NonMatching, "ParticleManager.c"),
            Object(NonMatching, "PhysicalObject.c"),
            Object(Matching, "Playback.c"),
            Object(NonMatching, "Player.c"),
            Object(Matching, "ProcessManager.c"),
            Object(NonMatching, "Projectile.c"),
            Object(NonMatching, "ShaderHolder.c"),
            Object(NonMatching, "ShowStats.c"),
            Object(Matching, "SmartProjectiles.c"),
            Object(NonMatching, "SoundSequence.c"),
            Object(NonMatching, "State_GamePlay.c"),
            Object(Matching, "State_Movie.c"),
            Object(NonMatching, "State_RenderTest.c"),
            Object(Matching, "State_Shutdown.c"),
            Object(NonMatching, "State_TitleScreen.c"),
            Object(NonMatching, "StateManager.c"),
            Object(NonMatching, "Thing.c"),
            Object(NonMatching, "ThingManager.c"),
            Object(NonMatching, "Trail.c"),
            Object(NonMatching, "Trigger.c"),
            Object(Matching, "VertexColorSeqManager.c"),
            Object(NonMatching, "VolumeManager.c"),
            Object(Matching, "WildCard.c"),
            Object(NonMatching, "World.c"),
            Object(NonMatching, "PlayerManager.c"),
            Object(NonMatching, "AI_Creature.c"),
            Object(NonMatching, "ViewManager.c"),
            Object(Matching, "Tracer.c"),
            Object(Matching, "LightManager.c"),
            Object(NonMatching, "State_GameManager.c"),
            Object(NonMatching, "State_ProgressionManager.c"),
            Object(NonMatching, "CarA_AR.c"),
            Object(NonMatching, "CarA_HW.c"),
            Object(NonMatching, "GameRules.c"),
            Object(NonMatching, "GameSpecificInterfaces.c"),
            Object(Matching, "FlashHolder.c"),
            Object(Matching, "BulletHoles.c"),
            Object(Matching, "Motion.c"),
            Object(NonMatching, "PPCArch.c", mw_version="GC/1.2.5n"),
            Object(NonMatching, "OS.c", mw_version="GC/1.2.5n"),
            Object(Matching, "DebuggerDriver.c", mw_version="GC/1.2.5n"),
            Object(NonMatching, "bgmath_NGC.c"),
            Object(Matching, "Debug_NGC.c"),
            Object(NonMatching, "Main_NGC.c"),
            Object(Matching, "MemoryManager.c"),
            Object(Matching, "Plane.c"),
            Object(NonMatching, "Random.c"),
            Object(NonMatching, "TimeHandler_NGC.c"),
            Object(NonMatching, "Anim.c"),
            Object(NonMatching, "FontManager.c"),
            Object(NonMatching, "Gfx_Anim.c"),
            Object(NonMatching, "Gfx_DisplayList.c"),
            Object(Matching, "Gfx_GeometryHandler.c"),
            Object(NonMatching, "Gfx_Main.c"),
            Object(NonMatching, "Gfx_TextureHandler.c"),
            Object(Matching, "MovieManager.c"),
            Object(Matching, "Render_Texture_.c"),
            Object(NonMatching, "Render_.c"),
            Object(NonMatching, "Render_Geometry_.c"),
            Object(Matching, "Render_Lines_.c"),
            Object(NonMatching, "Render_Matrix_.c"),
            Object(NonMatching, "Render_QuadTri_.c"),
            Object(NonMatching, "THPVideoDecode.c"),
            Object(Matching, "THPDraw.c"),
            Object(Matching, "THPPlay.c"),
            Object(NonMatching, "THPPlayer.c"),
            Object(Matching, "THPRead.c"),
            Object(NonMatching, "THPAudioDecode.c"),
            Object(NonMatching, "THPDec.c", mw_version="GC/1.2.5"),
            Object(Matching, "THPAudio.c", mw_version="GC/1.2.5n"),
            Object(NonMatching, "Voice_.c"),
            Object(NonMatching, "IT_.c"),
            Object(NonMatching, "MIDI_.c"),
            Object(Matching, "Music_.c"),
            Object(NonMatching, "RIFF_.c", mw_version="GC/3.0a3"),
            Object(NonMatching, "Audio_.c"),
            Object(NonMatching, "Voice.c"),
            Object(NonMatching, "IT.c"),
            Object(NonMatching, "MIDI.c"),
            Object(Matching, "Music.c"),
            Object(NonMatching, "Audio.c"),
            Object(NonMatching, "allsrc.c"),
            Object(Matching, "DataChunk.c"),
            Object(NonMatching, "FileManager.c"),
            Object(NonMatching, "FileManager_NGC.c"),
            Object(Matching, "Input.c"),
            Object(NonMatching, "Input_NGC.c")
        ],
    },
    {
        "lib": "main",
        "mw_version": config.linker_version,
        "cflags": cflags_base,
        "progress_category": "game",
        "objects": [
            Object(NonMatching, "auto/text_80093714.c"),
            Object(NonMatching, "auto/text_80096A6C_.c"),
            Object(NonMatching, "auto/text_80096EC0_.c"),
            Object(Matching, "auto/text_80097074_.cpp"),
            Object(Matching, "auto/text_80097BE8.cpp"),
        ],
    },
    {
        "lib": "SSystem",
        "mw_version": "GC/1.2.5",
        "cflags": cflags_base,
        "progress_category": "sdk",
        "objects": [
            Object(NonMatching, "OSAlarm.c", mw_version="GC/1.2.5n"),
            Object(Matching, "OSAlloc.c"),
            Object(Matching, "OSArena.c"),
            Object(Matching, "OSAudioSystem.c", mw_version="GC/1.2.5n"),
            Object(NonMatching, "OSCache.c", mw_version="GC/1.2.5n"),
            Object(NonMatching, "OSContext.c"),
            Object(Matching, "OSError.c", mw_version="GC/1.2.5n"),
            Object(NonMatching, "OSFont.c", mw_version="GC/1.2.5n"),
            Object(NonMatching, "OSInterrupt.c", mw_version="GC/1.2.5n"),
            Object(Matching, "OSLink.c"),
            Object(Matching, "OSMessage.c", mw_version="GC/1.2.5n"),
            Object(NonMatching, "OSMemory.c", mw_version="GC/1.2.5n"),
            Object(Matching, "OSMutex.c", mw_version="GC/1.2.5n"),
            Object(NonMatching, "OSReboot.c"),
            Object(NonMatching, "OSReset.c"),
            Object(Matching, "OSResetSW.c", mw_version="GC/1.2.5n"),
            Object(Matching, "OSRtc.c", mw_version="GC/1.2.5n"),
            Object(NonMatching, "OSSync.c"),
            Object(NonMatching, "OSThread.c", mw_version="GC/1.2.5n"),
            Object(NonMatching, "OSTime.c"),
            Object(Matching, "time.dolphin.c", mw_version="GC/1.2.5n"),
            Object(NonMatching, "__start.c"),
            Object(NonMatching, "__ppc_eabi_init.cpp"),
            Object(NonMatching, "db.c"),
            Object(NonMatching, "mtx.c"),
            Object(Matching, "mtx44.c"),
            Object(NonMatching, "vec.c"),
            Object(NonMatching, "dvdlow.c", mw_version="GC/1.2.5n"),
            Object(NonMatching, "dvdfs.c", mw_version="GC/1.2.5n"),
            Object(NonMatching, "dvd.c", mw_version="GC/1.2.5n"),
            Object(Matching, "dvdqueue.c", mw_version="GC/1.2.5n"),
            Object(Matching, "dvderror.c"),
            Object(Matching, "fstload.c", mw_version="GC/1.2.5n"),
            Object(NonMatching, "vi.c", mw_version="GC/1.2.5n"),
            Object(Matching, "Padclamp.c", mw_version="GC/1.2.5n"),
            Object(NonMatching, "Pad.c", mw_version="GC/1.2.5n"),
            Object(NonMatching, "etc/ai.c", mw_version="GC/1.2.5n"),
            Object(Matching, "ar.c", mw_version="GC/1.2.5n"),
            Object(Matching, "arq.c", mw_version="GC/1.2.5n"),
            Object(Matching, "AX.c"),
            Object(Matching, "AXAlloc.c", mw_version="GC/1.2.5n"),
            Object(NonMatching, "AXAux.c"),
            Object(NonMatching, "AXCL.c"),
            Object(NonMatching, "AXOut.c", mw_version="GC/1.2.5n"),
            Object(NonMatching, "AXSPB.c"),
            Object(NonMatching, "AXVPB.c"),
            Object(NonMatching, "AXComp.c"),
            Object(NonMatching, "DSPCode.c"),
            Object(Matching, "AXProf.c"),
            Object(Matching, "delay.c", mw_version="GC/1.2.5n"),
            Object(NonMatching, "axfx.c"),
            Object(NonMatching, "dsp.c", mw_version="GC/1.2.5n"),
            Object(Matching, "dsp_debug.c"),
            Object(NonMatching, "dsp_task.c", mw_version="GC/1.2.5n"),
            Object(Matching, "CARDBios.c", mw_version="GC/1.2.5n"),
            Object(NonMatching, "CARDUnlock.c", mw_version="GC/1.2.5n"),
            Object(NonMatching, "CARDNet.c"),
            Object(Matching, "CARDRdwr.c", mw_version="GC/1.2.5n"),
            Object(NonMatching, "CARDBlock.c", mw_version="GC/1.2.5n"),
            Object(NonMatching, "CARDDir.c", mw_version="GC/1.2.5n"),
            Object(NonMatching, "CARDCheck.c", mw_version="GC/1.2.5n"),
            Object(NonMatching, "CARDMount.c", mw_version="GC/1.2.5n"),
            Object(Matching, "CARDFormat.c", mw_version="GC/1.2.5n"),
            Object(Matching, "CARDOpen.c", mw_version="GC/1.2.5n"),
            Object(Matching, "CARDCreate.c"),
            Object(Matching, "CARDRead.c", mw_version="GC/1.2.5n"),
            Object(Matching, "CARDWrite.c", mw_version="GC/1.2.5n"),
            Object(Matching, "CARDDelete.c", mw_version="GC/1.2.5n"),
            Object(Matching, "CARDStat.c", mw_version="GC/1.2.5n"),
            Object(NonMatching, "GXInit.c"),
            Object(Matching, "GXFifo.c", mw_version="GC/1.2.5n"),
            Object(NonMatching, "GXAttr.c", mw_version="GC/1.2.5n"),
            Object(Matching, "GXMisc.c", mw_version="GC/1.2.5n"),
            Object(Matching, "GXGeometry.c"),
            Object(NonMatching, "GXFrameBuf.c"),
            Object(Matching, "GXLight.c"),
            Object(Matching, "GXTexture.c", mw_version="GC/1.2.5n"),
            Object(Matching, "GXBump.c", mw_version="GC/1.2.5n"),
            Object(Matching, "GXTev.c", mw_version="GC/1.2.5n"),
            Object(NonMatching, "GXPixel.c", mw_version="GC/1.2.5n"),
            Object(NonMatching, "GXDraw.c"),
            Object(Matching, "GXStubs.c"),
            Object(Matching, "GXDisplayList.c", mw_version="GC/1.2.5n"),
            Object(NonMatching, "GXTransform.c"),
            Object(NonMatching, "GXPerf.c"),
            Object(NonMatching, "SIBios.c", mw_version="GC/1.2.5n"),
            Object(Matching, "SISamplingRate.c", mw_version="GC/1.2.5n"),
            Object(Matching, "SISteering.c", mw_version="GC/1.2.5n"),
            Object(Matching, "SISteeringXfer.c", mw_version="GC/1.2.5n"),
            Object(Matching, "SISteeringAuto.c", mw_version="GC/1.2.5n"),
            Object(NonMatching, "EXIBios.c", mw_version="GC/1.2.5n"),
            Object(Matching, "EXIUart.c"),
            Object(Matching, "mainloop.c", mw_version="GC/1.3"),
            Object(Matching, "nubevent.c", mw_version="GC/1.3"),
            Object(NonMatching, "nubinit.c", mw_version="GC/1.3"),
            Object(Matching, "msg.c", mw_version="GC/1.3.2r"),
            Object(Matching, "msgbuf.c", mw_version="GC/2.0"),
            Object(Matching, "serpoll.c", mw_version="GC/2.6"),
            Object(NonMatching, "usr_put.c"),
            Object(Matching, "dispatch.c", mw_version="GC/1.3"),
            Object(NonMatching, "msghndlr.c", mw_version="GC/2.0"),
            Object(NonMatching, "support.c", mw_version="GC/1.3"),
            Object(Matching, "mutex_TRK.c"),
            Object(Matching, "notify.c", mw_version="GC/1.3"),
            Object(NonMatching, "flush_cache.c"),
            Object(Matching, "mem_TRK.c", mw_version="GC/1.3"),
            Object(NonMatching, "targimpl.c"),
            Object(NonMatching, "targsupp.s"),
            Object(NonMatching, "__exception.s"),
            Object(NonMatching, "dolphin_trk.c", mw_version="GC/1.3"),
            Object(NonMatching, "mpc_7xx_603e.c"),
            Object(Matching, "main_TRK.c", mw_version="GC/1.3"),
            Object(NonMatching, "dolphin_trk_glue.c", mw_version="GC/1.3"),
            Object(Matching, "targcont.c", mw_version="GC/1.3"),
            Object(Matching, "target_options.c"),
            Object(Matching, "mslsupp.c", mw_version="GC/1.3"),
            Object(Matching, "__va_arg.c", mw_version="GC/1.3.2r"),
            Object(NonMatching, "global_destructor_chain.c", mw_version="GC/1.3"),
            Object(NonMatching, "New.cp"),
            Object(NonMatching, "NewMore.cp"),
            Object(NonMatching, "NMWException.cp"),
            Object(NonMatching, "runtime.c"),
            Object(NonMatching, "__init_cpp_exceptions.cpp", mw_version="GC/1.3"),
            Object(NonMatching, "Gecko_ExceptionPPC.cp"),
            Object(Matching, "GCN_mem_alloc.c", mw_version="GC/1.3"),
            Object(Matching, "abort_exit.c", mw_version="GC/1.3"),
            Object(Matching, "alloc.c", mw_version="GC/1.3"),
            Object(NonMatching, "errno.c"),
            Object(Matching, "ansi_files.c", mw_version="GC/1.3"),
            Object(Matching, "ansi_fp.c", mw_version="GC/1.3"),
            Object(Matching, "buffer_io.c", mw_version="GC/1.3"),
            Object(Matching, "ctype.c", mw_version="GC/1.3"),
            Object(Matching, "direct_io.c", mw_version="GC/1.3"),
            Object(Matching, "file_io.c", mw_version="GC/1.3"),
            Object(Matching, "FILE_POS.C", mw_version="GC/1.3"),
            Object(Matching, "mbstring.c", mw_version="GC/1.3"),
            Object(Matching, "mem.c", mw_version="GC/1.3"),
            Object(NonMatching, "mem_funcs.c", mw_version="GC/1.3"),
            Object(Matching, "misc_io.c"),
            Object(Matching, "printf.c", mw_version="GC/1.3"),
            Object(NonMatching, "rand.c"),
            Object(NonMatching, "float.c"),
            Object(Matching, "signal.c", mw_version="GC/1.3"),
            Object(Matching, "string.c", mw_version="GC/1.3"),
            Object(Matching, "time.c", mw_version="GC/1.3"),
            Object(Matching, "uart_console_io.c", mw_version="GC/1.3"),
            Object(Matching, "wchar_io.c"),
            Object(Matching, "e_acos.c", mw_version="GC/1.3"),
            Object(Matching, "e_atan2.c", mw_version="GC/1.3"),
            Object(NonMatching, "e_pow.c"),
            Object(NonMatching, "e_rem_pio2.c"),
            Object(Matching, "k_cos.c", mw_version="GC/1.3"),
            Object(Matching, "k_rem_pio2.c", mw_version="GC/1.3"),
            Object(Matching, "k_sin.c", mw_version="GC/1.3"),
            Object(NonMatching, "k_tan.c"),
            Object(Matching, "s_atan.c", mw_version="GC/1.3"),
            Object(Matching, "s_copysign.c", mw_version="GC/1.3"),
            Object(Matching, "s_cos.c", mw_version="GC/1.3"),
            Object(Matching, "s_floor.c", mw_version="GC/1.3"),
            Object(Matching, "s_frexp.c", mw_version="GC/1.3"),
            Object(Matching, "s_ldexp.c", mw_version="GC/1.3"),
            Object(Matching, "s_modf.c", mw_version="GC/1.3"),
            Object(Matching, "s_sin.c", mw_version="GC/1.3"),
            Object(Matching, "s_tan.c", mw_version="GC/1.3"),
            Object(Matching, "w_acos.c", mw_version="GC/1.3"),
            Object(Matching, "w_atan2.c", mw_version="GC/1.3"),
            Object(Matching, "w_pow.c", mw_version="GC/1.3"),
            Object(Matching, "math_ppc.c", mw_version="GC/1.3"),
            Object(Matching, "AmcExi2Stubs.c"),
            Object(Matching, "odenotstub.c"),
            Object(NonMatching, "etc/gba.c", mw_version="GC/1.3"),
            Object(Matching, "Constant.cpp", mw_version="GC/1.3"),
            Object(NonMatching, "Force.cpp", mw_version="GC/1.3"),
            Object(Matching, "LGWheels.cpp", mw_version="GC/1.3.2r"),
            Object(Matching, "Periodic.cpp", mw_version="GC/1.3"),
            Object(Matching, "Ramp.cpp", mw_version="GC/1.3"),
            Object(NonMatching, "Wheels.cpp", mw_version="GC/1.3"),
            Object(NonMatching, "Conditon.cpp", mw_version="GC/1.3"),
            Object(NonMatching, "GBA.c"),
            Object(Matching, "GBAGetProcessStatus.c"),
            Object(NonMatching, "GBAJoyBoot.c", mw_version="GC/1.2.5n"),
            Object(Matching, "GBARead.c"),
            Object(Matching, "GBAWrite.c", mw_version="GC/1.2.5n"),
            Object(Matching, "GBAXfer.c", mw_version="GC/1.2.5n"),
            Object(NonMatching, "GBAKey.c", mw_version="GC/1.2.5n")
        ],
    },
]


# Optional callback to adjust link order. This can be used to add, remove, or reorder objects.
# This is called once per module, with the module ID and the current link order.
#
# For example, this adds "dummy.c" to the end of the DOL link order if configured with --non-matching.
# "dummy.c" *must* be configured as a Matching (or Equivalent) object in order to be linked.
def link_order_callback(module_id: int, objects: List[str]) -> List[str]:
    # Don't modify the link order for matching builds
    if not config.non_matching:
        return objects
    if module_id == 0:  # DOL
        return objects + ["dummy.c"]
    return objects


# Uncomment to enable the link order callback.
# config.link_order_callback = link_order_callback


# Optional extra categories for progress tracking
# Adjust as desired for your project
config.progress_categories = [
    ProgressCategory("game", "Game Code"),
    ProgressCategory("sdk", "SDK Code"),
]
config.progress_each_module = args.verbose
# Optional extra arguments to `objdiff-cli report generate`
config.progress_report_args = [
    # Marks relocations as mismatching if the target value is different
    # Default is "functionRelocDiffs=none", which is most lenient
    # "--config functionRelocDiffs=data_value",
]

if args.mode == "configure":
    # Write build.ninja and objdiff.json
    generate_build(config)
elif args.mode == "progress":
    # Print progress information
    calculate_progress(config)
else:
    sys.exit("Unknown mode: " + args.mode)
