#include "plugin.hpp"
#include "expanded/Compare.hpp"
#include "expanded/PhraseSeq16.hpp"

struct ExpandThis : Module {
	enum ParamId {
		K1_PARAM,
		K2_PARAM,
		B1_PARAM,
		B2_PARAM,
		PARAMS_LEN
	};
	enum InputId {
		INPUTS_LEN
	};
	enum OutputId {
		OUTPUTS_LEN
	};
	enum LightId {
		LIGHTS_LEN
	};

	dsp::BooleanTrigger Trigger1;
	dsp::BooleanTrigger Trigger2;
	Compare* compare;
	PhraseSeq16* ps16;
	bool isCompare = false;
	bool isPs16 = false;

	ExpandThis() {
		config(PARAMS_LEN, INPUTS_LEN, OUTPUTS_LEN, LIGHTS_LEN);
		configParam(K1_PARAM, 0.f, 1.f, 0.f, "K1_PARAM");
		configParam(K2_PARAM, 0.f, 10.f, 0.f, "K2_PARAM");
		configParam(B1_PARAM, 0.f, 1.f, 0.f, "B1_PARAM");
		configParam(B2_PARAM, 0.f, 1.f, 0.f, "B2_PARAM");
	}

	void process(const ProcessArgs& args) override {
		if (Trigger1.process(params[B1_PARAM].getValue())) {
			DEBUG("Button 1 trigger");

			bool expanderPresent = (this->leftExpander.module);
			if (expanderPresent) {
				DEBUG("Expander is present");
				DEBUG("%s", this->leftExpander.module->getModel()->getFullName().c_str());
				if (this->leftExpander.module->getModel()->getFullName() == "VCV Compare") {
					DEBUG("A compare!");
					if (!isCompare) {
						compare = reinterpret_cast<Compare*>(this->leftExpander.module);
						if (compare) {
							DEBUG("Cast OK");
							isCompare = true;
							isPs16 = false;
						}
					}
				}
				if (this->leftExpander.module->getModel()->getFullName() == "Impromptu PhraseSeq16") {
					DEBUG("A Ps16!");
					if (!isPs16) {
						ps16 = reinterpret_cast<PhraseSeq16*>(this->leftExpander.module);
						if (ps16) {
							DEBUG("Cast OK");
							isPs16 = true;
							isCompare = false;
						}
					}
				}
			} else {
				isCompare = false;
				isPs16 = false;
			}
		}
		if (Trigger2.process(params[B2_PARAM].getValue())) {
			DEBUG("Button 2 trigger");
			if (isCompare) {
				DEBUG("Setting division to %u", (uint32_t)(params[K1_PARAM].getValue() * 4096 * 2));
				compare->lightDivider.setDivision((uint32_t)(params[K1_PARAM].getValue() * 4096 * 2));
			}
			if (isPs16) {
				DEBUG("Ps16! Running state: %d Current step: %d", ps16->running, ps16->stepIndexRun);
			}
		}
		if (isCompare) {
			if (compare->lightDivider.getClock() == compare->lightDivider.getDivision() - 1) {
				// DEBUG("ClockDivider will trigger next (%u %u %f)", compare->lightDivider.getClock(), compare->lightDivider.getDivision(), compare->params[Compare::B_PARAM].getValue());
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
		addParam(createParamCentered<LEDButton>(mm2px(Vec(15.444, 80.603)), module, ExpandThis::B1_PARAM));
		addParam(createParamCentered<LEDButton>(mm2px(Vec(30.282, 80.603)), module, ExpandThis::B2_PARAM));
	}
};


Model* modelExpandThis = createModel<ExpandThis, ExpandThisWidget>("ExpandThis");