// ObsAppTest.cpp : This file contains the 'main' function. Program execution begins and ends there.
//

#include <iostream>
#include <sstream>
#include <vector>

#include "include/obs.hpp"
#include "include/util/util.hpp"
#include "include/util/platform.h"
#include <stdlib.h>

#include "StreamMgr.h"

using namespace std;

//vector<OBSSource> scourcesArr;
//void AddScene(OBSSource source)
//{
//	const char *name = obs_source_get_name(source);
//	obs_scene_t *scene = obs_scene_from_source(source);
//	scourcesArr.emplace_back(source);
//
//	signal_handler_t *handler = obs_source_get_signal_handler(source);
//
//
//	///* if the scene already has items (a duplicated scene) add them */
//	auto addSceneItem = [](obs_sceneitem_t *item) {
//		//AddSceneItem(item);
//	};
//
//	//using addSceneItem_t = decltype(addSceneItem);
//
//	obs_scene_enum_items(
//		scene,
//		[](obs_scene_t *, obs_sceneitem_t *item, void *param) {
//		//addSceneItem_t *func;
//		//func = reinterpret_cast<addSceneItem_t *>(param);
//		//(*func)(item);
//		return true;
//	},
//		&addSceneItem);
//
//	//SaveProject();
//}


//void setConfigDefaults(ConfigFile& basicConfig)
//{
//	config_set_default_string(basicConfig, "Output", "Mode", "Simple");
//
//	config_set_default_string(basicConfig, "SimpleOutput", "RecFormat",
//		"mkv");
//	config_set_default_uint(basicConfig, "SimpleOutput", "VBitrate", 2500);
//	config_set_default_uint(basicConfig, "SimpleOutput", "ABitrate", 160);
//	config_set_default_bool(basicConfig, "SimpleOutput", "UseAdvanced",
//		false);
//	config_set_default_bool(basicConfig, "SimpleOutput", "EnforceBitrate",
//		true);
//	config_set_default_string(basicConfig, "SimpleOutput", "Preset",
//		"veryfast");
//	config_set_default_string(basicConfig, "SimpleOutput", "NVENCPreset",
//		"hq");
//	config_set_default_string(basicConfig, "SimpleOutput", "RecQuality",
//		"Stream");
//	config_set_default_bool(basicConfig, "SimpleOutput", "RecRB", false);
//	config_set_default_int(basicConfig, "SimpleOutput", "RecRBTime", 20);
//	config_set_default_int(basicConfig, "SimpleOutput", "RecRBSize", 512);
//	config_set_default_string(basicConfig, "SimpleOutput", "RecRBPrefix",
//		"Replay");
//
//	config_set_default_bool(basicConfig, "AdvOut", "ApplyServiceSettings",
//		true);
//	config_set_default_bool(basicConfig, "AdvOut", "UseRescale", false);
//	config_set_default_uint(basicConfig, "AdvOut", "TrackIndex", 1);
//	config_set_default_uint(basicConfig, "AdvOut", "VodTrackIndex", 2);
//	config_set_default_string(basicConfig, "AdvOut", "Encoder", "obs_x264");
//
//	config_set_default_string(basicConfig, "AdvOut", "RecType", "Standard");
//
//	config_set_default_string(basicConfig, "AdvOut", "RecFormat", "mkv");
//	config_set_default_bool(basicConfig, "AdvOut", "RecUseRescale", false);
//	config_set_default_uint(basicConfig, "AdvOut", "RecTracks", (1 << 0));
//	config_set_default_string(basicConfig, "AdvOut", "RecEncoder", "none");
//	config_set_default_uint(basicConfig, "AdvOut", "FLVTrack", 1);
//
//	config_set_default_bool(basicConfig, "AdvOut", "FFOutputToFile", true);
//	config_set_default_string(basicConfig, "AdvOut", "FFExtension", "mp4");
//	config_set_default_uint(basicConfig, "AdvOut", "FFVBitrate", 2500);
//	config_set_default_uint(basicConfig, "AdvOut", "FFVGOPSize", 250);
//	config_set_default_bool(basicConfig, "AdvOut", "FFUseRescale", false);
//	config_set_default_bool(basicConfig, "AdvOut", "FFIgnoreCompat", false);
//	config_set_default_uint(basicConfig, "AdvOut", "FFABitrate", 160);
//	config_set_default_uint(basicConfig, "AdvOut", "FFAudioMixes", 1);
//
//	config_set_default_uint(basicConfig, "AdvOut", "Track1Bitrate", 160);
//	config_set_default_uint(basicConfig, "AdvOut", "Track2Bitrate", 160);
//	config_set_default_uint(basicConfig, "AdvOut", "Track3Bitrate", 160);
//	config_set_default_uint(basicConfig, "AdvOut", "Track4Bitrate", 160);
//	config_set_default_uint(basicConfig, "AdvOut", "Track5Bitrate", 160);
//	config_set_default_uint(basicConfig, "AdvOut", "Track6Bitrate", 160);
//
//	config_set_default_bool(basicConfig, "AdvOut", "RecRB", false);
//	config_set_default_uint(basicConfig, "AdvOut", "RecRBTime", 20);
//	config_set_default_int(basicConfig, "AdvOut", "RecRBSize", 512);
//
//	config_set_default_uint(basicConfig, "Video", "BaseCX", 1920);
//	config_set_default_uint(basicConfig, "Video", "BaseCY", 1080);
//
//	config_set_default_string(basicConfig, "Output", "FilenameFormatting",
//		"%CCYY-%MM-%DD %hh-%mm-%ss");
//
//	config_set_default_bool(basicConfig, "Output", "DelayEnable", false);
//	config_set_default_uint(basicConfig, "Output", "DelaySec", 20);
//	config_set_default_bool(basicConfig, "Output", "DelayPreserve", true);
//
//	config_set_default_bool(basicConfig, "Output", "Reconnect", true);
//	config_set_default_uint(basicConfig, "Output", "RetryDelay", 10);
//	config_set_default_uint(basicConfig, "Output", "MaxRetries", 20);
//
//	config_set_default_string(basicConfig, "Output", "BindIP", "default");
//	config_set_default_bool(basicConfig, "Output", "NewSocketLoopEnable",
//		false);
//	config_set_default_bool(basicConfig, "Output", "LowLatencyEnable",
//		false);
//
//	int i = 0;
//	uint32_t scale_cx = 1280;
//	uint32_t scale_cy = 720;
//
//	config_set_default_uint(basicConfig, "Video", "OutputCX", scale_cx);
//	config_set_default_uint(basicConfig, "Video", "OutputCY", scale_cy);
//
//	/* don't allow OutputCX/OutputCY to be susceptible to defaults
//	 * changing */
//	if (!config_has_user_value(basicConfig, "Video", "OutputCX") ||
//		!config_has_user_value(basicConfig, "Video", "OutputCY")) {
//		config_set_uint(basicConfig, "Video", "OutputCX", scale_cx);
//		config_set_uint(basicConfig, "Video", "OutputCY", scale_cy);
//		config_save_safe(basicConfig, "tmp", nullptr);
//	}
//
//	config_set_default_uint(basicConfig, "Video", "FPSType", 0);
//	config_set_default_string(basicConfig, "Video", "FPSCommon", "30");
//	config_set_default_uint(basicConfig, "Video", "FPSInt", 30);
//	config_set_default_uint(basicConfig, "Video", "FPSNum", 30);
//	config_set_default_uint(basicConfig, "Video", "FPSDen", 1);
//	config_set_default_string(basicConfig, "Video", "ScaleType", "bicubic");
//	config_set_default_string(basicConfig, "Video", "ColorFormat", "NV12");
//	config_set_default_string(basicConfig, "Video", "ColorSpace", "709");
//	config_set_default_string(basicConfig, "Video", "ColorRange",
//		"Partial");
//
//	config_set_default_string(basicConfig, "Audio", "MonitoringDeviceId",
//		"default");
//	config_set_default_string(
//		basicConfig, "Audio", "MonitoringDeviceName",
//		"Basic.Settings.Advanced.Audio.MonitoringDevice"
//			".Default");
//	config_set_default_uint(basicConfig, "Audio", "SampleRate", 48000);
//	config_set_default_string(basicConfig, "Audio", "ChannelSetup",
//		"Stereo");
//	config_set_default_uint(basicConfig, "Audio", "PeakMeterType", 0);
//	config_set_default_string(basicConfig, "SimpleOutput", "StreamEncoder", "x264");
//}


//bool AddNew(const char *id, const char *name, const bool visible)
//{
//	OBSScene scene = curScene;
//	bool success = false;
//	if (!scene)
//		return false;
//
//	obs_source_t *source = obs_get_source_by_name(name);
//	if (source) {
//	}
//	else {
//		const char *v_id = obs_get_latest_input_type_id(id);
//		source = obs_source_create(v_id, name, NULL, nullptr);
//
//		if (source) {
//			AddSourceData data;
//			data.source = source;
//			data.visible = visible;
//
//			obs_enter_graphics();
//			obs_scene_atomic_update(scene, AddSource, &data);
//			obs_leave_graphics();
//
//			/* set monitoring if source monitors by default */
//			uint32_t flags = obs_source_get_output_flags(source);
//			if ((flags & OBS_SOURCE_MONITOR_BY_DEFAULT) != 0) {
//				obs_source_set_monitoring_type(
//					source,
//					OBS_MONITORING_TYPE_MONITOR_ONLY);
//			}
//
//			success = true;
//		}
//	}
//
//	obs_source_release(source);
//	return success;
//}


int main()
{
	StreamMgr mgr;

	if (mgr.Init() == true)
		cout << "OBS started Successfully." << endl;
	else
		cout << "OBS failed to start!" << endl;

	mgr.StartStreaming();

	//OBSData settings = obs_data_create();
	//obs_data_release(settings);
	//obs_data_set_string(settings, "service", "Twitch");
	//obs_data_set_string(settings, "server", "rtmp://live-mil.twitch.tv/app");
	//obs_data_set_string(settings, "key", "live_605862708_50voVVDVzdiTiauKeYCc9rjVT3Z3Rw");

	//OBSService service = obs_service_create("rtmp_common", "default_service", settings,	nullptr);

	//obs_service_release(service);


	cin.get();

	mgr.StopStreaming();

	int iii;
	cin >> iii;
}

// Run program: Ctrl + F5 or Debug > Start Without Debugging menu
// Debug program: F5 or Debug > Start Debugging menu

// Tips for Getting Started: 
//   1. Use the Solution Explorer window to add/manage files
//   2. Use the Team Explorer window to connect to source control
//   3. Use the Output window to see build output and other messages
//   4. Use the Error List window to view errors
//   5. Go to Project > Add New Item to create new code files, or Project > Add Existing Item to add existing code files to the project
//   6. In the future, to open this project again, go to File > Open > Project and select the .sln file
