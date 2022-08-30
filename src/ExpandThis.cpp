#include "plugin.hpp"
#include "Compare.hpp"

struct ExpandThis : Module {
	enum ParamId {
		K1_PARAM,
		K2_PARAM,
		// FMCV_PARAM,
		B1_PARAM,
		B2_PARAM,
		// PWMCV_PARAM,
		PARAMS_LEN
	};
	enum InputId {
		// FM_INPUT,
		// PITCH_INPUT,
		// SYNC_INPUT,
		// PWM_INPUT,
		INPUTS_LEN
	};
	enum OutputId {
		// SIN_OUTPUT,
		// TRI_OUTPUT,
		// SAW_OUTPUT,
		// SQR_OUTPUT,
		OUTPUTS_LEN
	};
	enum LightId {
		// FREQ_LIGHT,
		LIGHTS_LEN
	};

	dsp::BooleanTrigger Trigger1;
	dsp::BooleanTrigger Trigger2;
	Compare* compare;
	bool isCompare = false;

	ExpandThis() {
		config(PARAMS_LEN, INPUTS_LEN, OUTPUTS_LEN, LIGHTS_LEN);
		configParam(K1_PARAM, 0.f, 1.f, 0.f, "K1_PARAM");
		configParam(K2_PARAM, 0.f, 10.f, 0.f, "K2_PARAM");
		// configParam(FMCV_PARAM, 0.f, 1.f, 0.f, "");
		configParam(B1_PARAM, 0.f, 1.f, 0.f, "B1_PARAM");
		configParam(B2_PARAM, 0.f, 1.f, 0.f, "B2_PARAM");
		// configParam(PWMCV_PARAM, 0.f, 1.f, 0.f, "");
		// configInput(FM_INPUT, "");
		// configInput(PITCH_INPUT, "");
		// configInput(SYNC_INPUT, "");
		// configInput(PWM_INPUT, "");
		// configOutput(SIN_OUTPUT, "");
		// configOutput(TRI_OUTPUT, "");
		// configOutput(SAW_OUTPUT, "");
		// configOutput(SQR_OUTPUT, "");
	}

	void process(const ProcessArgs& args) override {
		if (Trigger1.process(params[B1_PARAM].getValue())) {
			DEBUG("yaaah!");

			bool expandablePresent = (this->leftExpander.module);
			if (expandablePresent) {
				DEBUG("Yes module!");
				if (this->leftExpander.module->model == modelCompare) {
					DEBUG("It's a modelCompare!!");
				} else {
					DEBUG("Nope, it's a %s", this->leftExpander.module->model->getFullName().c_str());
				}
				if (!isCompare) {
					compare = dynamic_cast<Compare*>(this->leftExpander.module);
					if (compare) {
						DEBUG("But then, yes!");
						isCompare = true;
					}
				}
			} else {
				isCompare = false;
			}
		}
		if (Trigger2.process(params[B2_PARAM].getValue())) {
			if (isCompare) {
				DEBUG("Hacking will start soon... ou pas!");

				// int newSeq = (int)( params[K2_PARAM].getValue() * (16.0f - 1.0f) / 10.0f + 0.5f );
				// int seqIndexEdit = clamp(newSeq, 0, 16 - 1);
				// DEBUG("Step index is %d", compare->stepIndexEdit);
				 // = seqIndexEdit;
			}
		}
	}
};


struct ExpandThisWidget : ModuleWidget {
	ExpandThisWidget(ExpandThis* module) {
		setModule(module);
		setPanel(createPanel(asset::plugin(pluginInstance, "res/dummy-panel.svg")));

		addChild(createWidget<ScrewSilver>(Vec(RACK_GRID_WIDTH, 0)));
		addChild(createWidget<ScrewSilver>(Vec(box.size.x - 2 * RACK_GRID_WIDTH, 0)));
		addChild(createWidget<ScrewSilver>(Vec(RACK_GRID_WIDTH, RACK_GRID_HEIGHT - RACK_GRID_WIDTH)));
		addChild(createWidget<ScrewSilver>(Vec(box.size.x - 2 * RACK_GRID_WIDTH, RACK_GRID_HEIGHT - RACK_GRID_WIDTH)));

		addParam(createParamCentered<RoundHugeBlackKnob>(mm2px(Vec(22.905, 29.808)), module, ExpandThis::K1_PARAM));
		addParam(createParamCentered<RoundLargeBlackKnob>(mm2px(Vec(22.862, 56.388)), module, ExpandThis::K2_PARAM));
		// addParam(createParamCentered<Trimpot>(mm2px(Vec(6.607, 80.603)), module, ExpandThis::FMCV_PARAM));
		addParam(createParamCentered<LEDButton>(mm2px(Vec(15.444, 80.603)), module, ExpandThis::B1_PARAM));
		addParam(createParamCentered<LEDButton>(mm2px(Vec(30.282, 80.603)), module, ExpandThis::B2_PARAM));
		// addParam(createParamCentered<Trimpot>(mm2px(Vec(39.118, 80.603)), module, ExpandThis::PWMCV_PARAM));

		// addInput(createInputCentered<PJ301MPort>(mm2px(Vec(6.607, 96.859)), module, ExpandThis::FM_INPUT));
		// addInput(createInputCentered<PJ301MPort>(mm2px(Vec(17.444, 96.859)), module, ExpandThis::PITCH_INPUT));
		// addInput(createInputCentered<PJ301MPort>(mm2px(Vec(28.282, 96.859)), module, ExpandThis::SYNC_INPUT));
		// addInput(createInputCentered<PJ301MPort>(mm2px(Vec(39.15, 96.859)), module, ExpandThis::PWM_INPUT));

		// addOutput(createOutputCentered<PJ301MPort>(mm2px(Vec(6.607, 113.115)), module, ExpandThis::SIN_OUTPUT));
		// addOutput(createOutputCentered<PJ301MPort>(mm2px(Vec(17.444, 113.115)), module, ExpandThis::TRI_OUTPUT));
		// addOutput(createOutputCentered<PJ301MPort>(mm2px(Vec(28.282, 113.115)), module, ExpandThis::SAW_OUTPUT));
		// addOutput(createOutputCentered<PJ301MPort>(mm2px(Vec(39.119, 113.115)), module, ExpandThis::SQR_OUTPUT));

		// addChild(createLightCentered<SmallLight<RedGreenBlueLight>>(mm2px(Vec(31.089, 16.428)), module, ExpandThis::FREQ_LIGHT));
	}
};


Model* modelExpandThis = createModel<ExpandThis, ExpandThisWidget>("ExpandThis");