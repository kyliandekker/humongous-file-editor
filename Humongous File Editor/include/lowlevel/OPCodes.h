#pragma once

#include <map>
#include <stdint.h>
#include <string>
#include <functional>
#include <vector>

namespace HumongousFileEditor
{
    namespace chunk_reader
    {
        struct bytecode_data;

        size_t extended_b_op(unsigned char* data, size_t data_size, std::vector<std::string>& return_data);
        size_t extended_w_op(unsigned char* data, size_t data_size, std::vector<std::string>& return_data);
        size_t jump_cmd(unsigned char* data, size_t data_size, std::vector<std::string>& return_data);
        size_t msg_cmd(unsigned char* data, size_t data_size, std::vector<std::string>& return_data);
        size_t msg_op(unsigned char* data, size_t data_size, std::vector<std::string>& return_data);
        size_t actor_ops_v6(unsigned char* data, size_t data_size, std::vector<std::string>& return_data);
        size_t verb_ops_v6(unsigned char* data, size_t data_size, std::vector<std::string>& return_data);
        size_t array_ops_v6(unsigned char* data, size_t data_size, std::vector<std::string>& return_data);
        size_t wait_ops(unsigned char* data, size_t data_size, std::vector<std::string>& return_data);
        size_t extended_bw_op(unsigned char* data, size_t data_size, std::vector<std::string>& return_data);

        size_t room_ops_he60(unsigned char* data, size_t data_size, std::vector<std::string>& return_data);
        size_t actor_ops_he60(unsigned char* data, size_t data_size, std::vector<std::string>& return_data);
        size_t dmsg_op(unsigned char* data, size_t data_size, std::vector<std::string>& return_data);

        size_t sys_msg(unsigned char* data, size_t data_size, std::vector<std::string>& return_data);

        // TODO: Figure out what this does.
        size_t ini_op_v71(unsigned char* data, size_t data_size, std::vector<std::string>& return_data);
        size_t array_ops(unsigned char* data, size_t data_size, std::vector<std::string>& return_data);

        size_t extended_dw_op(unsigned char* data, size_t data_size, std::vector<std::string>& return_data);

        size_t file_op(unsigned char* data, size_t data_size, std::vector<std::string>& return_data);

        size_t extended_ww_op(unsigned char* data, size_t data_size, std::vector<std::string>& return_data);

        size_t default_func(unsigned char* data, size_t data_size, std::vector<std::string>& return_data);

        struct bytecode
        {
            std::string name;
            std::function<size_t(unsigned char*, size_t, std::vector<std::string>&)> func = default_func;
        };

        inline std::map<uint8_t, bytecode> OPCODES_HE90 =
        {
            // HE90
            { 0x0A, { "o90_dup_n", extended_w_op } },
            { 0x1C, { "o90_wizImageOps", extended_b_op} },
            { 0x1D, { "o90_min"} },
            { 0x1E, { "o90_max"} },
            { 0x1F, { "o90_sin"} },
            { 0x20, { "o90_cos"} },
            { 0x21, { "o90_sqrt"} },
            { 0x22, { "o90_atan2"} },
            { 0x23, { "o90_getSegmentAngle"} },
            { 0x24, { "o90_getDistanceBetweenPoints", extended_b_op} },
            { 0x25, { "o90_getSpriteInfo", extended_b_op} },
            { 0x26, { "o90_setSpriteInfo", extended_b_op} },
            { 0x27, { "o90_getSpriteGroupInfo", extended_b_op} },
            { 0x28, { "o90_setSpriteGroupInfo", extended_b_op} },
            { 0x29, { "o90_getWizData", extended_b_op} },
            { 0x2A, { "o90_getActorData"} },
            { 0x2B, { "o90_startScriptUnk", extended_b_op} },
            { 0x2C, { "o90_jumpToScriptUnk", extended_b_op} },
            // TODO : { 0x2d, { "o90_videoOps"} },
            // TODO : { 0x2e, { "o90_getVideoData"} },
            { 0x2F, { "o90_floodFill", extended_b_op} },
            { 0x30, { "o90_mod"} },
            { 0x31, { "o90_shl"} },
            { 0x32, { "o90_shr"} },
            // TODO : { 0x33, { "o90_xor"} },
            { 0x34, { "o90_findAllObjectsWithClassOf"} },
            { 0x35, { "o90_getPolygonOverlap"} },
            { 0x36, { "o90_cond"} },
            { 0x37, { "o90_dim2dim2Array", extended_bw_op} },
            { 0x38, { "o90_redim2dimArray", extended_bw_op} },
            { 0x39, { "o90_getLinesIntersectionPoint", extended_ww_op} },
            { 0x3A, { "o90_sortArray", extended_bw_op} },
            { 0x44, { "o90_getObjectData", extended_b_op} },
            { 0x69, { "disabled_windowOps", extended_b_op} },
            { 0x94, { "o90_getPaletteData", extended_b_op} },
            { 0x9E, { "o90_paletteOps", extended_b_op} },
            { 0xA5, { "o90_fontUnk", extended_b_op} },
            // TODO : { 0xab, { "o90_getActorAnimProgress"} },
            // TODO : { 0xc8, { "o90_kernelGetFunctions"} },
            // TODO : { 0xc9, { "o90_kernelSetFunctions"} },,

            // HE80
            { 0x45, { "o80_createSound", extended_b_op} },
            { 0x46, { "o80_getFileSize"} },
            { 0x48, { "o80_stringToInt"} },
            { 0x49, { "o80_getSoundVar"} },
            { 0x4A, { "o80_localizeArrayToRoom"} },
            // TODO : { 0x4C, { "o80_sourceDebug"} },
            { 0x4D, { "o80_readConfigFile", extended_b_op} },
            { 0x4E, { "o80_writeConfigFile", extended_b_op} },
            { 0x69, { "", 0 } },
            { 0x6b, { "o80_cursorCommand", extended_b_op} },
            { 0x70, { "o80_setState"} },
            { 0x76, { "", 0 } },
            { 0x94, { "", 0 } },
            { 0x9E, { "", 0 } },
            { 0xA5, { "", 0 } },
            { 0xAC, { "o80_drawWizPolygon"} },
            { 0xE0, { "o80_drawLine", extended_b_op} },
            { 0xE3, { "o80_pickVarRandom", extended_w_op} },

            // HE73
            { 0xF8, { "o73_getResourceSize", extended_b_op} },

            // HE72
            { 0x02, { "o72_pushDWord", extended_dw_op} },
            { 0x04, { "o72_getScriptString", msg_op} },
            { 0x0A, { "", 0 } },
            { 0x1B, { "o72_isAnyOf"} },
            { 0x42, { "", 0 } },
            { 0x46, { "", 0 } },
            { 0x4A, { "", 0 } },
            { 0x4E, { "", 0 } },
            { 0x50, { "o72_resetCutscene"} },
            { 0x51, { "o72_getHeap", extended_b_op} },
            { 0x52, { "o72_findObjectWithClassOf"} },
            { 0x54, { "o72_getObjectImageX"} },
            { 0x55, { "o72_getObjectImageY"} },
            { 0x56, { "o72_captureWizImage"} },
            { 0x58, { "o72_getTimer", extended_b_op} },
            { 0x59, { "o72_setTimer", extended_b_op} },
            { 0x5A, { "o72_getSoundPosition"} },
            { 0x5E, { "o72_startScript", extended_b_op} },
            { 0x60, { "o72_startObject", extended_b_op} },
            { 0x61, { "o72_drawObject", extended_b_op} },
            { 0x62, { "o72_printWizImage"} },
            { 0x63, { "o72_getArrayDimSize", extended_bw_op} },
            { 0x64, { "o72_getNumFreeArrays"} },
            { 0x97, { "", 0 } },
            { 0x9C, { "o72_roomOps", extended_b_op} },
            { 0x9D, { "o72_actorOps", extended_b_op} },
            { 0x9E, { "o72_verbOps", extended_b_op} },
            // TODO : { 0xa0, { "o72_findObject"} },
            { 0xA4, { "o72_arrayOps", array_ops} },
            { 0xAE, { "o72_systemOps", extended_b_op} },
            { 0xBA, { "o72_talkActor", msg_op} },
            { 0xBB, { "o72_talkEgo", msg_op} },
            { 0xBC, { "o72_dimArray", extended_bw_op} },
            { 0xC0, { "o72_dim2dimArray", extended_bw_op} },
            { 0xC1, { "o72_traceStatus"} },
            { 0xC8, { "o72_kernelGetFunctions"} },
            { 0xCE, { "o72_drawWizImage"} },
            { 0xCF, { "o72_debugInput"} },
            { 0xD5, { "o72_jumpToScript", extended_b_op} },
            { 0xDA, { "o72_openFile"} },
            { 0xDB, { "o72_readFile", file_op} },
            { 0xDC, { "o72_writeFile", file_op} },
            { 0xDD, { "o72_findAllObjects"} },
            { 0xDE, { "o72_deleteFile"} },
            { 0xDF, { "o72_rename"} },
            { 0xE1, { "o72_getPixel", extended_b_op} },
            // TODO : { 0xe3, { "o72_pickVarRandom"} },
            { 0xEA, { "o72_redimArray", extended_bw_op} },
            { 0xF3, { "o72_readINI", extended_b_op} },
            { 0xF4, { "o72_writeINI", extended_b_op} },
            { 0xF8, { "o72_getResourceSize"} },
            { 0xF9, { "o72_createDirectory"} },
            { 0xFA, { "o72_setSystemMessage", extended_b_op} },

            // HE71
            { 0xC9, { "o71_kernelSetFunctions"} },
            { 0xEC, { "o71_copyString"} },
            { 0xED, { "o71_getStringWidth"} },
            { 0xEF, { "o71_appendString"} },
            { 0xF0, { "o71_concatString"} },
            { 0xF1, { "o71_compareString"} },
            { 0xF5, { "o71_getStringLenForWidth"} },
            { 0xF6, { "o71_getCharIndexInString"} },
            // TODO : { 0xf7, { "o71_findBox"} },
            { 0xFB, { "o71_polygonOps", extended_b_op} },
            { 0xFC, { "o71_polygonHit"} },

            // HE70
            { 0x74, { "o70_soundOps", extended_b_op} },
            { 0x84, { "o70_pickupObject"} },
            { 0x8C, { "o70_getActorRoom"} },
            { 0x9B, { "o70_resourceRoutines", extended_b_op} },
            { 0xAE, { "o70_systemOps", extended_b_op} },
            { 0xEE, { "o70_getStringLen"} },
            { 0xF2, { "o70_isResourceLoaded", extended_b_op} },
            { 0xF3, { "o70_readINI", msg_op} },
            { 0xF4, { "o70_writeINI", ini_op_v71} },
            { 0xF9, { "o70_createDirectory", msg_op} },
            { 0xFA, { "o70_setSystemMessage", sys_msg} },

            // HE60
            { 0x63, { "", 0 } },
            { 0x64, { "", 0 } },
            { 0x70, { "o60_setState"} },
            { 0x9A, { "", 0 } },
            { 0x9C, { "o60_roomOps", room_ops_he60} },
            { 0x9D, { "o60_actorOps", actor_ops_he60} },
            { 0xAC, { "", 0 } },
            { 0xBD, { "o6_stopObjectCodeReturn"} },
            { 0xC8, { "o60_kernelGetFunctions"} },
            { 0xC9, { "o60_kernelSetFunctions"} },
            { 0xD9, { "o60_closeFile"} },
            { 0xDA, { "o60_openFile", msg_op} },
            { 0xDB, { "o60_readFile"} },
            { 0xDC, { "o60_writeFile"} },
            { 0xDE, { "o60_deleteFile", msg_op} },
            { 0xDF, { "o60_rename", dmsg_op} },
            { 0xe0, { "o60_soundOps", extended_b_op} },
            { 0xE2, { "o60_localizeArrayToScript"} },
            { 0xE9, { "o60_seekFilePos"} },
            { 0xEA, { "o60_redimArray", extended_bw_op} },
            { 0xEB, { "o60_readFilePos"} },
            { 0xEC, { "", 0 } },
            { 0xED, { "", 0 } },

            // V6
            { 0x00, { "o6_pushByte", extended_b_op} },
            { 0x01, { "o6_pushWord", extended_w_op} },
            { 0x02, { "o6_pushByteVar", extended_b_op} },
            { 0x03, { "o6_pushWordVar", extended_w_op} },
            // TODO : { 0x06, { "o6_byteArrayRead"} },
            { 0x07, { "o6_wordArrayRead", extended_w_op} },
            { 0x0A, { "o6_byteArrayIndexedRead", extended_b_op} },
            { 0x0B, { "o6_wordArrayIndexedRead", extended_w_op} },
            { 0x0C, { "o6_dup"} },
            { 0x0D, { "o6_not"} },
            { 0x0E, { "o6_eq"} },
            { 0x0F, { "o6_neq"} },
            { 0x10, { "o6_gt"} },
            { 0x11, { "o6_lt"} },
            { 0x12, { "o6_le"} },
            { 0x13, { "o6_ge"} },
            { 0x14, { "o6_add"} },
            { 0x15, { "o6_sub"} },
            { 0x16, { "o6_mul"} },
            { 0x17, { "o6_div"} },
            { 0x18, { "o6_land"} },  // logical and
            { 0x19, { "o6_lor"} },  // logical or
            { 0x1A, { "o6_pop"} },
            // TODO : { 0x42, { "o6_writeByteVar"} },
            { 0x43, { "o6_writeWordVar", extended_w_op} },
            // TODO : { 0x46, { "o6_byteArrayWrite"} },
            { 0x47, { "o6_wordArrayWrite", extended_w_op} },
            // TODO : { 0x4a, { "o6_byteArrayIndexedWrite"} },
            { 0x4B, { "o6_wordArrayIndexedWrite", extended_w_op} },
            // TODO : { 0x4e, { "o6_byteVarInc"} },
            { 0x4F, { "o6_wordVarInc", extended_w_op} },
            // TODO : { 0x52, { "o6_byteArrayInc"} },
            { 0x53, { "o6_wordArrayInc", extended_w_op} },
            // TODO : { 0x56, { "o6_byteVarDec"} },
            { 0x57, { "o6_wordVarDec", extended_w_op} },
            // TODO : { 0x5a, { "o6_byteArrayDec"} },
            { 0x5B, { "o6_wordArrayDec", extended_w_op} },
            { 0x5C, { "o6_if", jump_cmd} },  // jump if
            { 0x5D, { "o6_ifNot", jump_cmd} },  // jump if not
            { 0x5E, { "o6_startScript"} },
            { 0x5F, { "o6_startScriptQuick"} },
            { 0x60, { "o6_startObject"} },
            { 0x61, { "o6_drawObject"} },
            { 0x62, { "o6_drawObjectAt"} },
            { 0x63, { "o6_drawBlastObject"} },
            { 0x64, { "o6_setBlastObjectWindow"} },
            { 0x65, { "o6_stopObjectCodeObject"} },  // o6_stopObjectCode
            { 0x66, { "o6_stopObjectCodeScript"} },  // o6_stopObjectCode
            { 0x67, { "o6_endCutscene"} },
            { 0x68, { "o6_cutscene"} },
            // TODO : { 0x69, { "o6_stopMusic"} },
            { 0x6A, { "o6_freezeUnfreeze"} },
            { 0x6B, { "o6_cursorCommand", extended_b_op} },
            { 0x6C, { "o6_breakHere"} },
            { 0x6D, { "o6_ifClassOfIs"} },
            { 0x6E, { "o6_setClass"} },
            { 0x6F, { "o6_getState"} },
            { 0x70, { "o6_setState"} },
            { 0x71, { "o6_setOwner"} },
            { 0x72, { "o6_getOwner"} },
            { 0x73, { "o6_jump", jump_cmd} },
            { 0x74, { "o6_startSound"} },
            { 0x75, { "o6_stopSound"} },
            // TODO : { 0x76, { "o6_startMusic"} },
            { 0x77, { "o6_stopObjectScript"} },
            { 0x78, { "o6_panCameraTo"} },
            { 0x79, { "o6_actorFollowCamera"} },
            { 0x7A, { "o6_setCameraAt"} },
            { 0x7B, { "o6_loadRoom"} },
            { 0x7C, { "o6_stopScript"} },
            { 0x7D, { "o6_walkActorToObj"} },
            { 0x7E, { "o6_walkActorTo"} },
            { 0x7F, { "o6_putActorAtXY"} },
            { 0x80, { "o6_putActorAtObject"} },
            { 0x81, { "o6_faceActor"} },
            { 0x82, { "o6_animateActor"} },
            { 0x83, { "o6_doSentence"} },
            { 0x84, { "o6_pickupObject"} },
            { 0x85, { "o6_loadRoomWithEgo"} },
            { 0x87, { "o6_getRandomNumber"} },
            { 0x88, { "o6_getRandomNumberRange"} },
            { 0x8A, { "o6_getActorMoving"} },
            { 0x8B, { "o6_isScriptRunning"} },
            { 0x8C, { "o6_getActorRoom"} },
            { 0x8D, { "o6_getObjectX"} },
            { 0x8E, { "o6_getObjectY"} },
            { 0x8F, { "o6_getObjectOldDir"} },
            { 0x90, { "o6_getActorWalkBox"} },
            { 0x91, { "o6_getActorCostume"} },
            { 0x92, { "o6_findInventory"} },
            { 0x93, { "o6_getInventoryCount"} },
            { 0x94, { "o6_getVerbFromXY"} },
            { 0x95, { "o6_beginOverride"} },
            { 0x96, { "o6_endOverride"} },
            { 0x97, { "o6_setObjectName", msg_op} },
            { 0x98, { "o6_isSoundRunning"} },
            { 0x99, { "o6_setBoxFlags"} },
            { 0x9A, { "o6_createBoxMatrix"} },
            { 0x9B, { "o6_resourceRoutines", extended_b_op} },
            { 0x9C, { "o6_roomOps", extended_b_op} },
            { 0x9D, { "o6_actorOps", actor_ops_v6} },
            { 0x9E, { "o6_verbOps", verb_ops_v6} },
            { 0x9F, { "o6_getActorFromXY"} },
            { 0xA0, { "o6_findObject"} },
            { 0xA1, { "o6_pseudoRoom"} },
            { 0xA2, { "o6_getActorElevation"} },
            { 0xA3, { "o6_getVerbEntrypoint"} },
            { 0xA4, { "o6_arrayOps", array_ops_v6} },
            { 0xA5, { "o6_saveRestoreVerbs", extended_b_op} },
            { 0xA6, { "o6_drawBox"} },
            { 0xA7, { "o6_pop"} },
            { 0xA8, { "o6_getActorWidth"} },
            { 0xA9, { "o6_wait", wait_ops} },
            { 0xAA, { "o6_getActorScaleX"} },
            { 0xAB, { "o6_getActorAnimCounter"} },
            { 0xAC, { "o6_soundKludge"} },
            { 0xAD, { "o6_isAnyOf"} },
            { 0xAE, { "o6_systemOps", extended_b_op} },
            { 0xAF, { "o6_isActorInBox"} },
            { 0xB0, { "o6_delay"} },
            { 0xB1, { "o6_delaySeconds"} },
            { 0xB2, { "o6_delayMinutes"} },
            { 0xB3, { "o6_stopSentence"} },
            { 0xB4, { "o6_printLine", msg_cmd} },
            { 0xB5, { "o6_printText", msg_cmd} },
            { 0xB6, { "o6_printDebug", msg_cmd} },
            { 0xB7, { "o6_printSystem", msg_cmd} },
            { 0xB8, { "o6_printActor", msg_cmd} },
            { 0xB9, { "o6_printEgo", msg_cmd} },
            { 0xBA, { "o6_talkActor", msg_op} },
            { 0xBB, { "o6_talkEgo", msg_op} },
            { 0xBC, { "o6_dimArray", extended_bw_op} },
            { 0xBD, { "o6_dummy"} },
            { 0xBE, { "o6_startObjectQuick"} },
            { 0xBF, { "o6_startScriptQuick2"} },
            { 0xC0, { "o6_dim2dimArray", extended_bw_op} },
            { 0xC4, { "o6_abs"} },
            { 0xC5, { "o6_distObjectObject"} },
            // TODO : { 0xc6, { "o6_distObjectPt"} },
            { 0xC7, { "o6_distPtPt"} },
            { 0xC8, { "o6_kernelGetFunctions"} },
            { 0xC9, { "o6_kernelSetFunctions"} },
            { 0xCA, { "o6_delayFrames"} },
            { 0xCB, { "o6_pickOneOf"} },
            { 0xCC, { "o6_pickOneOfDefault"} },
            { 0xCD, { "o6_stampObject"} },
            { 0xD0, { "o6_getDateTime"} },
            { 0xD1, { "o6_stopTalking"} },
            { 0xD2, { "o6_getAnimateVariable"} },
            { 0xD4, { "o6_shuffle", extended_w_op} },
            { 0xD5, { "o6_jumpToScript"} },
            { 0xD6, { "o6_band"} },  // bitwise and
            { 0xD7, { "o6_bor"} },  // bitwise or
            { 0xD8, { "o6_isRoomScriptRunning"} },
            { 0xDD, { "o6_findAllObjects"} },
            { 0xE1, { "o6_getPixel"} },
            { 0xE3, { "o6_pickVarRandom", extended_w_op} },
            { 0xE4, { "o6_setBoxSet", extended_b_op} },
            { 0xEC, { "o6_getActorLayer"} },
            { 0xED, { "o6_getObjectNewDir"} },
        };
    }
}