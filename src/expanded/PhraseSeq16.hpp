#include "PhraseSeqUtil.hpp"
#include "comp/PianoKey.hpp"

struct PhraseSeq16 : Module {
	enum ParamIds {
		KEYNOTE_PARAM,
		KEYGATE_PARAM,
		KEYGATE2_PARAM,// no longer unused
		EDIT_PARAM,
		SEQUENCE_PARAM,
		RUN_PARAM,
		COPY_PARAM,
		PASTE_PARAM,
		RESET_PARAM,
		ENUMS(OCTAVE_PARAM, 7),
		GATE1_PARAM,
		GATE2_PARAM,
		SLIDE_BTN_PARAM,
		SLIDE_KNOB_PARAM,
		ATTACH_PARAM,
		ROTATEL_PARAM,// no longer used
		ROTATER_PARAM,// no longer used
		PASTESYNC_PARAM,// no longer used
		AUTOSTEP_PARAM,
		ENUMS(KEY_PARAMS, 12),// no longer used
		TRANSPOSEU_PARAM,// no longer used
		TRANSPOSED_PARAM,// no longer used
		RUNMODE_PARAM,
		TRAN_ROT_PARAM,
		ROTATE_PARAM,//no longer used
		GATE1_KNOB_PARAM,
		GATE2_KNOB_PARAM,// no longer used
		GATE1_PROB_PARAM,
		TIE_PARAM,// Legato
		CPMODE_PARAM,
		ENUMS(STEP_PHRASE_PARAMS, 16),
		NUM_PARAMS
	};
	enum InputIds {
		WRITE_INPUT,
		CV_INPUT,
		RESET_INPUT,
		CLOCK_INPUT,
		LEFTCV_INPUT,
		RIGHTCV_INPUT,
		RUNCV_INPUT,
		SEQCV_INPUT,
		NUM_INPUTS
	};
	enum OutputIds {
		CV_OUTPUT,
		GATE1_OUTPUT,
		GATE2_OUTPUT,
		NUM_OUTPUTS
	};
	enum LightIds {
		ENUMS(STEP_PHRASE_LIGHTS, 16 * 3),// room for GreenRedWhite
		ENUMS(OCTAVE_LIGHTS, 7),// octaves 1 to 7
		ENUMS(KEY_LIGHTS, 12 * 2),// room for GreenRed
		RUN_LIGHT,
		RESET_LIGHT,
		ENUMS(GATE1_LIGHT, 2),// room for GreenRed
		ENUMS(GATE2_LIGHT, 2),// room for GreenRed
		SLIDE_LIGHT,
		ATTACH_LIGHT,
		PENDING_LIGHT,// no longer used
		ENUMS(GATE1_PROB_LIGHT, 2),// room for GreenRed
		TIE_LIGHT,
		KEYNOTE_LIGHT,
		ENUMS(KEYGATE_LIGHT, 2),// room for GreenRed
		NUM_LIGHTS
	};
	
	
	// Expander
	float rightMessages[2][5] = {};// messages from expander


	// Constants
	enum DisplayStateIds {DISP_NORMAL, DISP_MODE, DISP_LENGTH, DISP_TRANSPOSE, DISP_ROTATE};


	// Need to save, no reset
	int panelTheme;
	float panelContrast;
	
	// Need to save, with reset
	bool autoseq;
	bool autostepLen;
	bool holdTiedNotes;
	bool keepCVTiedNotes;
	bool allowSlideTies;
	int seqCVmethod;// 0 is 0-10V, 1 is C4-D5#, 2 is TrigIncr
	int pulsesPerStep;// 1 means normal gate mode, alt choices are 4, 6, 12, 24 PPS (Pulses per step)
	bool running;
	int runModeSong;
	int stepIndexEdit;
	int seqIndexEdit;
	int phraseIndexEdit;
	int phrases;//1 to 16
	SeqAttributes sequences[16];
	int phrase[16];// This is the song (series of phases; a phrase is a patten number)
	float cv[16][16];// [-3.0 : 3.917]. First index is patten number, 2nd index is step
	StepAttributes attributes[16][16];// First index is patten number, 2nd index is step (see enum AttributeBitMasks for details)
	bool resetOnRun;
	bool attached;
	bool stopAtEndOfSong;

	// No need to save, with reset
	int displayState;
	float cvCPbuffer[16];// copy paste buffer for CVs
	StepAttributes attribCPbuffer[16];
	int phraseCPbuffer[16];
	SeqAttributes seqAttribCPbuffer;
	bool seqCopied;
	int countCP;// number of steps to paste (in case CPMODE_PARAM changes between copy and paste)
	int startCP;
	unsigned long editingGate;// 0 when no edit gate, downward step counter timer when edit gate
	unsigned long editingType;// similar to editingGate, but just for showing remanent gate type (nothing played); uses editingGateKeyLight
	long infoCopyPaste;// 0 when no info, positive downward step counter timer when copy, negative upward when paste
	unsigned long clockPeriod;// counts number of step() calls upward from last clock (reset after clock processed)
	long tiedWarning;// 0 when no warning, positive downward step counter timer when warning
	long attachedWarning;// 0 when no warning, positive downward step counter timer when warning
	long revertDisplay;
	long editingGateLength;// 0 when no info, positive when gate1, negative when gate2
	long lastGateEdit;
	long editingPpqn;// 0 when no info, positive downward step counter timer when editing ppqn
	long clockIgnoreOnReset;
	int phraseIndexRun;
	unsigned long phraseIndexRunHistory;
	int stepIndexRun;
	unsigned long stepIndexRunHistory;
	int ppqnCount;
	int gate1Code;
	int gate2Code;
	bool lastProbGate1Enable;
	unsigned long slideStepsRemain;// 0 when no slide under way, downward step counter when sliding


	// No need to save, no reset
	RefreshCounter refresh;
	float slideCVdelta;// no need to initialize, this goes with slideStepsRemain
	float editingGateCV;// no need to initialize, this goes with editingGate (output this only when editingGate > 0)
	int editingGateKeyLight;// no need to initialize, this goes with editingGate (use this only when editingGate > 0)
	float resetLight = 0.0f;
	int sequenceKnob = 0;
	Trigger resetTrigger;
	Trigger leftTrigger;
	Trigger rightTrigger;
	Trigger runningTrigger;
	Trigger clockTrigger;
	Trigger octTriggers[7];
	Trigger octmTrigger;
	Trigger gate1Trigger;
	Trigger gate1ProbTrigger;
	Trigger gate2Trigger;
	Trigger slideTrigger;
	dsp::BooleanTrigger keyTrigger;
	Trigger writeTrigger;
	Trigger attachedTrigger;
	Trigger copyTrigger;
	Trigger pasteTrigger;
	Trigger modeTrigger;
	Trigger rotateTrigger;
	Trigger transposeTrigger;
	Trigger tiedTrigger;
	Trigger stepTriggers[16];
	Trigger keyNoteTrigger;
	Trigger keyGateTrigger;
	Trigger seqCVTrigger;
	HoldDetect modeHoldDetect;
	PianoKeyInfo pkInfo;

	
	inline bool isEditingSequence(void) ;
	
	
	PhraseSeq16() ;
	

	void onReset() override;
	void resetNonJson();
	void initRun();
	
	
	void onRandomize() override;
	
	
	json_t *dataToJson() override;

	void dataFromJson(json_t *rootJ) override;


	IoStep* fillIoSteps(int *seqLenPtr);
	
	
	void emptyIoSteps(IoStep* ioSteps, int seqLen);
	
	
	void rotateSeq(int seqNum, bool directionRight, int seqLength);
	
	
	void calcGate1Code(StepAttributes attribute);
	

	void process(const ProcessArgs &args) override;// process()
	

	inline void setGreenRed(int id, float green, float red);

	inline void propagateCVtoTied(int seqn, int stepn);

	void activateTiedStep(int seqn, int stepn);
	
	void deactivateTiedStep(int seqn, int stepn);
	
	inline void setGateLight(bool gateOn, int lightIndex);
	
};

// struct PhraseSeq16Widget : ModuleWidget {
// 	struct SequenceDisplayWidget : TransparentWidget {
// 		PhraseSeq16 *module;
// 		std::shared_ptr<Font> font;
// 		std::string fontPath;
// 		char displayStr[16];
// 		int lastNum = -1;// -1 means timedout; >= 0 means we have a first number potential, if ever second key comes fast enough
// 		clock_t lastTime = 0;
		
// 		SequenceDisplayWidget();
		
// 		void onHoverKey(const event::HoverKey& e) override;

		
// 		void runModeToStr(int num);

// 		void drawLayer(const DrawArgs &args, int layer) override;
// 	};		
	
// 	struct InteropSeqItem : MenuItem {
// 		struct InteropCopySeqItem : MenuItem {
// 			PhraseSeq16 *module;
// 			void onAction(const event::Action &e) override;
// 		};
// 		struct InteropPasteSeqItem : MenuItem {
// 			PhraseSeq16 *module;
// 			void onAction(const event::Action &e) override;
// 		};
// 		PhraseSeq16 *module;
// 		Menu *createChildMenu() override;
// 	};		
	
// 	void appendContextMenu(Menu *menu) override;	
	
// 	struct SequenceKnob : IMBigKnobInf {
// 		SequenceKnob() {};		
// 		void onDoubleClick(const event::DoubleClick &e) override;
// 	};	
	
	
// 	PhraseSeq16Widget(PhraseSeq16 *module);
// };