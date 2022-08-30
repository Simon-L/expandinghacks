struct Compare : Module {
	enum ParamId {
		B_PARAM,
		PARAMS_LEN
	};
	enum InputId {
		A_INPUT,
		B_INPUT,
		INPUTS_LEN
	};
	enum OutputId {
		MAX_OUTPUT,
		MIN_OUTPUT,
		CLIP_OUTPUT,
		LIM_OUTPUT,
		CLIPGATE_OUTPUT,
		LIMGATE_OUTPUT,
		GREATER_OUTPUT,
		LESS_OUTPUT,
		OUTPUTS_LEN
	};
	enum LightId {
		ENUMS(CLIP_LIGHT, 2),
		ENUMS(LIM_LIGHT, 2),
		ENUMS(GREATER_LIGHT, 2),
		ENUMS(LESS_LIGHT, 2),
		LIGHTS_LEN
	};

	dsp::ClockDivider lightDivider;

	void process(const ProcessArgs& args) override;
};


struct CompareWidget : ModuleWidget {
	CompareWidget(Compare* module);
};

Model* modelCompare = createModel<Compare, CompareWidget>("Compare");