#include "StreamMgr.h"

#include "include/util/util.hpp"
#include "include/util/platform.h"

StreamMgr::StreamMgr()
{
}

StreamMgr::~StreamMgr()
{
}

void StreamMgr::shutDown()
{
	obs_shutdown();
}

struct AddSourceData {
	obs_source_t *source;
	bool visible;
};
static void AddSource(void *_data, obs_scene_t *scene)
{
	AddSourceData *data = (AddSourceData *)_data;
	obs_sceneitem_t *sceneitem;

	sceneitem = obs_scene_add(scene, data->source);
	obs_sceneitem_set_visible(sceneitem, data->visible);
}

void StreamMgr::SourceCreated(void *data, calldata_t *params)
{
	obs_source_t *source = (obs_source_t *)calldata_ptr(params, "source");
	StreamMgr* pMgr = static_cast<StreamMgr *>(data);
	if (source != NULL)
		pMgr->scourcesArr.emplace_back(source);
}

bool StreamMgr::Init()
{
	std::string strPath = "./plugin_config";

	if (obs_startup("en-US", strPath.c_str(), NULL) == false)
		return false;

	if (InitVideo() == false)
		return false;
	if (InitAudio() == false)
		return false;

	OBSSignal sig(obs_get_signal_handler(), "source_create", StreamMgr::SourceCreated, this);

	obs_load_all_modules();
	obs_log_loaded_modules();
	obs_post_load_modules();

	if (CreateEncoders() == false)
		return false;

	CreateDefaultScene();
	if(!os_file_exists("./scenes.json"))
		Save("./scenes.json");

	if (LoadSources() == false)
		return false;

	return true;
}

bool StreamMgr::InitVideo()
{
	struct obs_video_info ovi;

	ovi.fps_num = 60;
	ovi.fps_den = 1;

	const char *colorFormat = "NV12";
	const char *colorSpace = "709";
	const char *colorRange = "Partial";

	ovi.graphics_module = "libobs-d3d11.dll";
	ovi.base_width = 1920;
	ovi.base_height = 1080;
	ovi.output_width = 740;
	ovi.output_height = 426;
	ovi.output_format = VIDEO_FORMAT_NV12;
	ovi.colorspace = VIDEO_CS_709;
	ovi.range = VIDEO_RANGE_FULL;//VIDEO_RANGE_PARTIAL;
	ovi.adapter = 0; // first video device
	ovi.gpu_conversion = true;
	ovi.scale_type = OBS_SCALE_BICUBIC;

	if (ovi.base_width < 8 || ovi.base_height < 8) {
		ovi.base_width = 1920;
		ovi.base_height = 1080;
	}

	if (ovi.output_width < 8 || ovi.output_height < 8) {
		ovi.output_width = ovi.base_width;
		ovi.output_height = ovi.base_height;
	}

	int ret = obs_reset_video(&ovi);
	if (ret != OBS_VIDEO_SUCCESS) {
		if (ret == OBS_VIDEO_CURRENTLY_ACTIVE) {
			return false;
		}

		/* Try OpenGL if DirectX fails on windows */
		if (_strcmpi(ovi.graphics_module, "libobs-opengl") != 0) {
			//cout << "Failed to initialize obs video (%d) " << "with graphics_module='%s', retrying " << "with graphics_module='%s'";
			ovi.graphics_module = "libobs-opengl.dll";
			ret = obs_reset_video(&ovi);
		}
	}

	if (ret == OBS_VIDEO_SUCCESS)
		return true;
	
	return false;
}

bool StreamMgr::InitAudio()
{
	struct obs_audio_info ai;
	ai.samples_per_sec = 48000;

	ai.speakers = SPEAKERS_STEREO;

	if (obs_reset_audio(&ai))
		return true;
	
	return false;
}

bool StreamMgr::CreateEncoders()
{
	h264Streaming = obs_video_encoder_create("obs_x264", "simple_h264_stream", nullptr, nullptr);

	if (!h264Streaming)
		return false;

	obs_encoder_release(h264Streaming);

	aacStreaming = obs_audio_encoder_create("ffmpeg_aac", "simple_aac", nullptr, 0, nullptr);

	if (!aacStreaming)
		return false;

	obs_encoder_release(aacStreaming);

	return true;
}

void StreamMgr::ResetAudioDevice(const char *sourceId, const char *deviceId,const char *deviceDesc, int channel)
{
	obs_source_t *source;
	obs_data_t *settings;

	settings = obs_data_create();
	obs_data_set_string(settings, "device_id", deviceId);
	source = obs_source_create(sourceId, deviceDesc, settings,
		nullptr);
	obs_data_release(settings);

	obs_set_output_source(channel, source);
	obs_source_release(source);
}

void StreamMgr::CreateDefaultScene()
{
	for (int i = 0; i < MAX_CHANNELS; i++)
		obs_set_output_source(i, nullptr);

	InitDefaultTransitions();

	SetTransition(fadeTransition);

	obs_scene_t *scene = obs_scene_create("Scene");

	curScene = scene;

	ResetAudioDevice("wasapi_output_capture", "default", "DesktopDevice1", 1);
	ResetAudioDevice("wasapi_input_capture", "default", "AuxDevice1", 3);

	obs_source_t *source = obs_scene_get_source(scene);
	OBSSource transition = obs_get_output_source(0);
	obs_source_release(transition);

	obs_transition_set(transition, source);

	obs_scene_release(scene);

	obs_source_t *source3 = obs_source_create("wasapi_input_capture", "Audio Input Capture", NULL, nullptr);
	AddSourceData data;
	data.source = source3;
	data.visible = true;
	obs_enter_graphics();
	obs_scene_atomic_update(curScene, AddSource, &data);
	obs_leave_graphics();
	obs_source_release(source3);

	OBSData settings = obs_data_create();
	obs_data_release(settings);
	obs_data_set_string(settings, "last_video_device_id","Microsoft® LifeCam HD-3000:\\\\?\\usb#22vid_045e&pid_0779&mi_00#226&383a58bc&0&0000#22{65e8773d-8f56-11d0-a3b9-00a0c9223196}\\global");
	obs_data_set_string(settings, "video_device_id", "Microsoft® LifeCam HD-3000:\\\\?\\usb#22vid_045e&pid_0779&mi_00#226&383a58bc&0&0000#22{65e8773d-8f56-11d0-a3b9-00a0c9223196}\\global");
	obs_source_t *source2 = obs_source_create("dshow_input", "Video Capture Device", settings, nullptr);
	//AddSourceData data;
	data.source = source2;
	data.visible = true;
	obs_enter_graphics();
	obs_scene_atomic_update(curScene, AddSource, &data);
	obs_leave_graphics();
	obs_set_output_source(2, source2);
	obs_source_release(source2);
}

static void SaveAudioDevice(const char *name, int channel, obs_data_t *parent,
	std::vector<OBSSource> &audioSources)
{
	obs_source_t *source = obs_get_output_source(channel);
	if (!source)
		return;

	audioSources.push_back(source);

	obs_data_t *data = obs_save_source(source);

	obs_data_set_obj(parent, name, data);

	obs_data_release(data);
	obs_source_release(source);
}

void StreamMgr::Save(const char *file)
{
	OBSScene scene = curScene;
	OBSSource curProgramScene = obs_scene_get_source(scene);

	obs_data_array_t *transitions = obs_data_array_create();

	for (int i = 0; i < Mytransitions.size(); i++) {
		OBSSource tr = Mytransitions[i];
		
		if (!tr || !obs_source_configurable(tr))
			continue;

		obs_data_t *sourceData = obs_data_create();
		obs_data_t *settings = obs_source_get_settings(tr);

		obs_data_set_string(sourceData, "name",
			obs_source_get_name(tr));
		obs_data_set_string(sourceData, "id", obs_obj_get_id(tr));
		obs_data_set_obj(sourceData, "settings", settings);

		obs_data_array_push_back(transitions, sourceData);

		obs_data_release(settings);
		obs_data_release(sourceData);
	}

	obs_data_t *saveData = obs_data_create();

	std::vector<OBSSource> audioSources;

	SaveAudioDevice(DESKTOP_AUDIO_1, 1, saveData, audioSources);
	SaveAudioDevice(AUX_AUDIO_1, 3, saveData, audioSources);

	obs_source_t *transition = obs_get_output_source(0);
	obs_source_t *currentScene = obs_scene_get_source(scene);
	const char *sceneName = obs_source_get_name(currentScene);
	const char *programName = obs_source_get_name(curProgramScene);

	/* -------------------------------- */
	/* save non-group sources           */

	auto FilterAudioSources = [&](obs_source_t *source) {
		if (obs_source_is_group(source))
			return false;

		return find(begin(audioSources), end(audioSources), source) ==
			end(audioSources);
	};
	using FilterAudioSources_t = decltype(FilterAudioSources);

	obs_data_array_t *sourcesArray = obs_save_sources_filtered(
		[](void *data, obs_source_t *source) {
		return (*static_cast<FilterAudioSources_t *>(data))(
			source);
	},
		static_cast<void *>(&FilterAudioSources));

	/* -------------------------------- */
	/* save group sources separately    */

	/* saving separately ensures they won't be loaded in older versions */
	obs_data_array_t *groupsArray = obs_save_sources_filtered(
		[](void *, obs_source_t *source) {
		return obs_source_is_group(source);
	}, nullptr);

	/* -------------------------------- */
	const char *sceneCollection = "Untitled";

	obs_data_set_string(saveData, "current_scene", sceneName);
	obs_data_set_string(saveData, "current_program_scene", programName);
	obs_data_set_string(saveData, "name", sceneCollection);
	obs_data_set_array(saveData, "sources", sourcesArray);
	obs_data_set_array(saveData, "groups", groupsArray);
	obs_data_set_array(saveData, "transitions", transitions);
	obs_data_array_release(sourcesArray);
	obs_data_array_release(groupsArray);

	obs_data_set_string(saveData, "current_transition",
		obs_source_get_name(transition));
	obs_data_set_int(saveData, "transition_duration", 300);
	obs_source_release(transition);
	
	obs_data_t *moduleObj = obs_data_create();
	//api->on_save(moduleObj);
	obs_data_set_obj(saveData, "modules", moduleObj);
	obs_data_release(moduleObj);

	if (!obs_data_save_json_safe(saveData, file, "tmp", "bak"))
		blog(LOG_ERROR, "Could not save scene data to %s", file);

	obs_data_release(saveData);
	obs_data_array_release(transitions);
}

bool StreamMgr::LoadSources()
{
	//obs_data_t *data =
	//	obs_data_create_from_json_file_safe("C:/Users/Admin/AppData/Roaming/obs-studio/basic/profiles/Untitled/service.json", "bak");

	//if (!data)
	//	return false;

	//obs_data_set_default_string(data, "type", "rtmp_common");
	//const char *type = obs_data_get_string(data, "type");

	//obs_data_t *settings = obs_data_get_obj(data, "settings");
	//obs_data_t *hotkey_data = obs_data_get_obj(data, "hotkeys");

	//service = obs_service_create(type, "default_service", settings,
	//	hotkey_data);
	//obs_service_release(service);

	//obs_data_release(hotkey_data);
	//obs_data_release(settings);
	//obs_data_release(data);

	for (int i = 0; i < MAX_CHANNELS; i++)
		obs_set_output_source(i, nullptr);

	InitDefaultTransitions();
	obs_data_t *data = obs_data_create_from_json_file_safe("./scenes.json", "bak");
	obs_data_t *modulesObj = obs_data_get_obj(data, "modules");
	obs_data_array_t *sceneOrder = obs_data_get_array(data, "scene_order");
	obs_data_array_t *sources = obs_data_get_array(data, "sources");
	obs_data_array_t *groups = obs_data_get_array(data, "groups");
	obs_data_array_t *transitions = obs_data_get_array(data, "transitions");
	const char *sceneName = obs_data_get_string(data, "current_scene");
	const char *programSceneName =
		obs_data_get_string(data, "current_program_scene");
	const char *transitionName =
		obs_data_get_string(data, "current_transition");

	long long newDuration = obs_data_get_int(data, "transition_duration");
	if (!newDuration)
		newDuration = 300;

	obs_data_set_default_string(data, "name", "Untitled");

	const char *name = obs_data_get_string(data, "name");
	obs_source_t *curProgramScene;
	obs_source_t *curTransition;

	LoadAudioDevice(DESKTOP_AUDIO_1, 1, data);
	LoadAudioDevice(DESKTOP_AUDIO_2, 2, data);
	LoadAudioDevice(AUX_AUDIO_1, 3, data);
	LoadAudioDevice(AUX_AUDIO_2, 4, data);
	LoadAudioDevice(AUX_AUDIO_3, 5, data);
	LoadAudioDevice(AUX_AUDIO_4, 6, data);

	if (!sources) {
		sources = groups;
		groups = nullptr;
	}
	else {
		obs_data_array_push_back_array(sources, groups);
	}

	obs_load_sources(sources, nullptr, nullptr);

	if (transitions)
		LoadTransitions(transitions);

	obs_data_array_release(transitions);

	curTransition = FindTransition(transitionName);
	obs_set_output_source(0, curTransition);
	SetTransition(curTransition);
	obs_source_t* curSceneLocal = obs_get_source_by_name("Scene");
	curProgramScene = obs_get_source_by_name("Scene");

	curScene = obs_scene_from_source(curSceneLocal);
	OBSSource transition = obs_get_output_source(0);
	obs_source_release(transition);

	float t = obs_transition_get_time(transition);
	bool stillTransitioning = t < 1.0f && t > 0.0f;

	obs_transition_set(transition, curSceneLocal);

	obs_source_release(curSceneLocal);
	obs_source_release(curProgramScene);

	obs_data_array_release(sources);
	obs_data_array_release(groups);
	obs_data_array_release(sceneOrder);

	return true;
}

void StreamMgr::SetTransition(OBSSource transition)
{
	obs_source_t *oldTransition = obs_get_output_source(0);
	obs_source_release(oldTransition);

	if (transition == oldTransition)
		return;

	if (oldTransition && transition) {
		obs_transition_swap_begin(transition, oldTransition);
		obs_set_output_source(0, transition);
		obs_transition_swap_end(transition, oldTransition);
	}
	else {
		obs_set_output_source(0, transition);
	}
}
void StreamMgr::LoadAudioDevice(const char *name, int channel, obs_data_t *parent)
{
	obs_data_t *data = obs_data_get_obj(parent, name);
	if (!data)
		return;

	obs_source_t *source = obs_load_source(data);
	if (source) {
		obs_set_output_source(channel, source);

		const char *name = obs_source_get_name(source);

		obs_monitoring_type monitoring_type = obs_source_get_monitoring_type(source);
		
		if (monitoring_type != OBS_MONITORING_TYPE_NONE) {
			const char *type = (monitoring_type == OBS_MONITORING_TYPE_MONITOR_ONLY)
				? "monitor only"
				: "monitor and output";
		}
		obs_source_release(source);
	}

	obs_data_release(data);
}

obs_source_t* StreamMgr::FindTransition(const char *name)
{
	for (int i = 0; i < Mytransitions.size(); i++) {
		OBSSource tr = Mytransitions[i];
		if (!tr)
			continue;

		const char *trName = obs_source_get_name(tr);
		if (trName && *trName && strcmp(trName, name) == 0)
			return tr;
	}

	return nullptr;
}

void StreamMgr::LoadTransitions(obs_data_array_t *transitions)
{
	size_t count = obs_data_array_count(transitions);

	for (size_t i = 0; i < count; i++) {
		obs_data_t *item = obs_data_array_item(transitions, i);
		const char *name = obs_data_get_string(item, "name");
		const char *id = obs_data_get_string(item, "id");
		obs_data_t *settings = obs_data_get_obj(item, "settings");

		obs_source_t *source =
			obs_source_create_private(id, name, settings);

		obs_data_release(settings);
		obs_data_release(item);
		obs_source_release(source);
	}
}

void StreamMgr::InitDefaultTransitions()
{
	size_t idx = 0;
	const char *id;

	/* automatically add transitions that have no configuration (things
	 * such as cut/fade/etc) */
	while (obs_enum_transition_types(idx++, &id)) {
		const char *name = obs_source_get_display_name(id);

		if (!obs_is_source_configurable(id)) {
			obs_source_t *tr =
				obs_source_create_private(id, name, NULL);
			Mytransitions.emplace_back(tr);
			if (strcmp(id, "fade_transition") == 0)
				fadeTransition = tr;
			else if (strcmp(id, "cut_transition") == 0)
				cutTransition = tr;
			obs_source_release(tr);
		}
	}
}

bool StreamMgr::StartStreaming()
{
	OBSData settings = obs_data_create();
	obs_data_release(settings);
	obs_data_set_string(settings, "service", "Twitch");
	obs_data_set_string(settings, "server", "rtmp://live-mil.twitch.tv/app");
	obs_data_set_string(settings, "key", "live_605862708_50voVVDVzdiTiauKeYCc9rjVT3Z3Rw");

	service = obs_service_create("rtmp_common", "default_service", settings,	nullptr);

	obs_service_release(service);
	obs_data_t *h264Settings = obs_data_create();
	obs_data_set_string(h264Settings, "rate_control", "CBR");
	obs_data_set_int(h264Settings, "bitrate", 2500);

	obs_data_t *aacSettings = obs_data_create();

	obs_data_set_string(aacSettings, "rate_control", "CBR");
	obs_data_set_int(aacSettings, "bitrate", 160);

	obs_service_apply_encoder_settings(service, h264Settings,
		aacSettings);

	obs_encoder_update(h264Streaming, h264Settings);
	obs_encoder_update(aacStreaming, aacSettings);

	obs_data_release(h264Settings);
	obs_data_release(aacSettings);

	obs_encoder_set_video(h264Streaming, obs_get_video());
	obs_encoder_set_audio(aacStreaming, obs_get_audio());

	streamOutput = obs_output_create("rtmp_output", "simple_stream", nullptr,
		nullptr);
	obs_output_release(streamOutput);

	obs_output_set_video_encoder(streamOutput, h264Streaming);
	obs_output_set_audio_encoder(streamOutput, aacStreaming, 0);
	obs_output_set_service(streamOutput, service);
	/////
	obs_output_set_delay(streamOutput, 0, OBS_OUTPUT_DELAY_PRESERVE);

	obs_output_set_reconnect_settings(streamOutput, 0, 10);

	return obs_output_start(streamOutput);
}

void StreamMgr::StopStreaming()
{
	obs_output_stop(streamOutput);
}
