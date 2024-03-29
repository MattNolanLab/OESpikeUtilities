
#include "SpikeDetectorDynamicEditor.h"
#include "SpikeDetectorDynamic.h"
#include <stdio.h>

SpikeDetectorDynamicEditor::SpikeDetectorDynamicEditor(GenericProcessor* parentNode, bool useDefaultParameterEditors = true)
    : GenericEditor(parentNode, useDefaultParameterEditors), isPlural(true)

{
	int silksize;
	const char* silk = CoreServices::getApplicationResource("silkscreenserialized", silksize);
    MemoryInputStream mis(silk, silksize, false);
    Typeface::Ptr typeface = new CustomTypeface(mis);
    font = Font(typeface);

    desiredWidth = 440;

    electrodeTypes = new ComboBox("Electrode Types");

	SpikeDetectorDynamic* processor = (SpikeDetectorDynamic*)getProcessor();

    for (int i = 0; i < processor->electrodeTypes.size(); i++)
    {
        String type = processor->electrodeTypes[i];
        electrodeTypes->addItem(type += "s", i+1);
    }

    electrodeTypes->setEditableText(false);
    electrodeTypes->setJustificationType(Justification::centredLeft);
    electrodeTypes->addListener(this);
    electrodeTypes->setBounds(65,40,110,20);
    electrodeTypes->setSelectedId(2);
    addAndMakeVisible(electrodeTypes);

    electrodeList = new ComboBox("Electrode List");
    electrodeList->setEditableText(false);
    electrodeList->setJustificationType(Justification::centredLeft);
    electrodeList->addListener(this);
    electrodeList->setBounds(15,75,115,20);
    addAndMakeVisible(electrodeList);

    numElectrodes = new Label("Number of Electrodes","1");
    numElectrodes->setEditable(true);
    numElectrodes->addListener(this);
    numElectrodes->setBounds(30,40,25,20);
    addAndMakeVisible(numElectrodes);

    upButton = new TriangleButton(1);
    upButton->addListener(this);
    upButton->setBounds(50,40,10,8);
    addAndMakeVisible(upButton);

    downButton = new TriangleButton(2);
    downButton->addListener(this);
    downButton->setBounds(50,50,10,8);
    addAndMakeVisible(downButton);

    plusButton = new UtilityButton("+", titleFont);
    plusButton->addListener(this);
    plusButton->setRadius(3.0f);
    plusButton->setBounds(15,42,14,14);
    addAndMakeVisible(plusButton);

    ElectrodeEditorButton* e1 = new ElectrodeEditorButton("EDIT",font);
    e1->addListener(this);
    addAndMakeVisible(e1);
    e1->setBounds(15,110,40,10);
    electrodeEditorButtons.add(e1);

    ElectrodeEditorButton* e2 = new ElectrodeEditorButton("MONITOR",font);
    e2->addListener(this);
    addAndMakeVisible(e2);
    e2->setBounds(55,110,70,10);
    electrodeEditorButtons.add(e2);

    ElectrodeEditorButton* e3 = new ElectrodeEditorButton("DELETE",font);
    e3->addListener(this);
    addAndMakeVisible(e3);
    e3->setBounds(130,110,70,10);
    electrodeEditorButtons.add(e3);

    thresholdSlider = new ThresholdSlider(font);
    thresholdSlider->setBounds(200,35,75,75);
	thresholdSlider->setRange(0, 10, 1);
	thresholdSlider->setMouseDragSensitivity(1);
    addAndMakeVisible(thresholdSlider);
    thresholdSlider->addListener(this);
    thresholdSlider->setActive(false);
    Array<double> v;
    thresholdSlider->setValues(v);

    thresholdLabel = new Label("Name","Threshold");
    font.setHeight(10);
    thresholdLabel->setFont(font);
    thresholdLabel->setBounds(202, 105, 95, 15);
    thresholdLabel->setColour(Label::textColourId, Colours::grey);
    addAndMakeVisible(thresholdLabel);

    // method of detection
    detectionMethod = new ComboBox("Detection method");
    detectionMethod->setJustificationType(Justification::centredLeft);
    detectionMethod->addListener(this);
    detectionMethod->setBounds(285, 35, 75, 20);


    for (int i = 0; i < processor->detectionMethod.size(); i++)
    {
        String method = processor->detectionMethod[i];
        detectionMethod->addItem(method, i+1);
    }

    detectionMethod->setSelectedId(2);
    addAndMakeVisible(detectionMethod);


    // detection sign

    detectionSign = new ComboBox("Detection sign");
    detectionSign->setJustificationType(Justification::centredLeft);
    detectionSign->addListener(this);
    for (int i = 0; i < processor->detectionSign.size(); i++)
    {
        String dSign = processor->detectionSign[i];
        detectionSign->addItem(dSign, i + 1);
    }
    detectionSign->setBounds(370, 35, 50, 20);
    detectionSign->setSelectedId(2);
    addAndMakeVisible(detectionSign);



    thresholdTextBox = new Label("thresholdText", "6");
    thresholdTextBox->setEditable(true);
    thresholdTextBox->setColour(Label::textColourId, Colours::white);
    thresholdTextBox->setColour(Label::backgroundColourId, Colours::grey);
    thresholdTextBox->setBounds(285, 65, 20, 20);
    addAndMakeVisible(thresholdTextBox);

    setAllThresholdBtn = new TextButton("Set all");
    setAllThresholdBtn->setBounds(315, 65, 40, 20);
    setAllThresholdBtn->addListener(this);
    addAndMakeVisible(setAllThresholdBtn);

    enableBtn = new ToggleButton("Enable");
    enableBtn->setBounds(285, 95, 75, 20);
    enableBtn->addListener(this);
    addAndMakeVisible(enableBtn);



    channelSelector->inactivateButtons();
    channelSelector->paramButtonsToggledByDefault(false);
}

SpikeDetectorDynamicEditor::~SpikeDetectorDynamicEditor()
{
    for (int i = 0; i < electrodeButtons.size(); i++)
    {
        removeChildComponent(electrodeButtons[i]);
    }
    deleteAllChildren();
}

void SpikeDetectorDynamicEditor::sliderEvent(Slider* slider)
{
    int electrodeNum = -1;
    for (int i = 0; i < electrodeButtons.size(); i++)
    {
        if (electrodeButtons[i]->getToggleState())
        {
            electrodeNum = i; 
            break;
        }
    }
    if (electrodeNum > -1)
    {
		SpikeDetectorDynamic* processor = (SpikeDetectorDynamic*)getProcessor();
        processor->setChannelThreshold(electrodeList->getSelectedItemIndex(),
                                       electrodeNum,
                                       slider->getValue());
    }
}

void SpikeDetectorDynamicEditor::buttonEvent(Button* button)
{
    if (electrodeButtons.contains((ElectrodeButton*) button))
    {
        if (electrodeEditorButtons[0]->getToggleState()) // EDIT is active
        {
            ElectrodeButton* eb = (ElectrodeButton*) button;
            int electrodeNum = eb->getChannelNum()-1;

            Array<int> a;
            a.add(electrodeNum);
            channelSelector->setActiveChannels(a);

			SpikeDetectorDynamic* processor = (SpikeDetectorDynamic*)getProcessor();

            thresholdSlider->setActive(true);
            thresholdSlider->setValue(processor->getChannelThreshold(electrodeList->getSelectedItemIndex(),
                                                                     electrodeButtons.indexOf((ElectrodeButton*) button)));
        }
        else
        {
			SpikeDetectorDynamic* processor = (SpikeDetectorDynamic*)getProcessor();

            ElectrodeButton* eb = (ElectrodeButton*) button;
            int electrodeNum = electrodeList->getSelectedItemIndex();
            int channelNum = electrodeButtons.indexOf(eb);

            processor->setChannelActive(electrodeNum,
                                        channelNum,
                                        button->getToggleState());

            std::cout << "Disabling channel " << channelNum <<
                      " of electrode " << electrodeNum << std::endl;

        }
    }

    int num = numElectrodes->getText().getIntValue();

    if (button == upButton)
    {
        numElectrodes->setText(String(++num), sendNotification);
        return;
    }
    else if (button == downButton)
    {
        if (num > 1)
            numElectrodes->setText(String(--num), sendNotification);
        return;
    }
    else if (button == plusButton)
    {
        if (acquisitionIsActive)
        {
            CoreServices::sendStatusMessage("Stop acquisition before adding electrodes.");
            return;
        }

        int type = electrodeTypes->getSelectedId();
        int nChans;

        switch (type)
        {
            case 1:
                nChans = 1;
                break;
            case 2:
                nChans = 2;
                break;
            case 3:
                nChans = 4;
                break;
            default:
                nChans = 1;
        }

        for (int n = 0; n < num; n++)
        {
            if (!addElectrode(nChans))
            {
                CoreServices::sendStatusMessage("Not enough channels to add electrode.");
            }
        }

        electrodeEditorButtons[1]->setToggleState(false, dontSendNotification);

		CoreServices::updateSignalChain(this);
		CoreServices::highlightEditor(this);
        return;

    }
    else if (button == electrodeEditorButtons[0])   // EDIT
    {
        Array<int> activeChannels;

        for (int i = 0; i < electrodeButtons.size(); i++)
        {
            if (button->getToggleState())
            {
                electrodeButtons[i]->setToggleState(false, dontSendNotification);
                electrodeButtons[i]->setRadioGroupId(299);
                channelSelector->activateButtons();
                channelSelector->setRadioStatus(true);
            }
            else
            {
                electrodeButtons[i]->setToggleState(true, dontSendNotification);
                electrodeButtons[i]->setRadioGroupId(0);
                channelSelector->inactivateButtons();
                channelSelector->setRadioStatus(false);
                activeChannels.add(electrodeButtons[i]->getChannelNum()-1);
            }
        }

        if (!button->getToggleState())
        {
            thresholdSlider->setActive(false);

            // This will be -1 with nothing selected
            int selectedItemIndex = electrodeList->getSelectedItemIndex();
            if (selectedItemIndex != -1)
            {
                drawElectrodeButtons(selectedItemIndex);
            }
            else
            {
                electrodeButtons.clear();
            }
        }
        return;
    }
    else if (button == electrodeEditorButtons[1])   // MONITOR
    {

        Button* audioMonitorButton = electrodeEditorButtons[1];
        channelSelector->clearAudio();
		SpikeDetectorDynamic* processor = (SpikeDetectorDynamic*)getProcessor();

		Array<SimpleElectrode*> electrodes;
		processor->getElectrodes(electrodes);

        for (int i = 0; i < electrodes.size(); i++)
        {
            SimpleElectrode* e = electrodes[i];
            e->isMonitored = false;
        }

        SimpleElectrode* e = processor->getActiveElectrode();

        if (e != nullptr)
        {
            e->isMonitored = audioMonitorButton->getToggleState();
            for (int i = 0; i < e->numChannels; i++)
            {
                std::cout << "Channel " << e->channels[i] << std::endl;
                int channelNum = e->channels[i];
                channelSelector->setAudioStatus(channelNum, audioMonitorButton->getToggleState());
            }
        }
        else
        {
            audioMonitorButton->setToggleState(false, dontSendNotification);
        }
        return;
    }
    else if (button == electrodeEditorButtons[2])   // DELETE
    {
        if (acquisitionIsActive)
        {
            CoreServices::sendStatusMessage("Stop acquisition before deleting electrodes.");
            return;
        }
        removeElectrode(electrodeList->getSelectedItemIndex());

		CoreServices::updateSignalChain(this);
		CoreServices::highlightEditor(this);
        return;
    }
    else if (button == setAllThresholdBtn) {
        //set all threshold to be the same number

        double threshold = double(thresholdTextBox->getTextValue().getValue());

        Array<double> thresholds;

        SpikeDetectorDynamic* processor = (SpikeDetectorDynamic*)getProcessor();

        //loop through all electrode and all channel in the electrode to set threshold
        for (int i = 0; i < electrodeList->getNumItems(); i++) {
            for (int j = 0; j < electrodeButtons.size(); j++) {
                thresholds.add(threshold);
               processor->setChannelThreshold(i,
                    j,
                    threshold);
            }
            
        }

        thresholdSlider->setValues(thresholds);
        thresholdSlider->repaint(); //force refresh display

    }
    else if (button == enableBtn) { //enabled detection

        bool isEnableDetection = enableBtn->getToggleState();
        SpikeDetectorDynamic* processor = (SpikeDetectorDynamic*)getProcessor();
        processor->setEnableDetection(isEnableDetection);
    }
}

void SpikeDetectorDynamicEditor::channelChanged(int channel, bool /*newState*/)
{

    if (electrodeEditorButtons[0]->getToggleState()) // editing is active
    {
        //std::cout << "New channel: " << chan << std::endl;

        for (int i = 0; i < electrodeButtons.size(); i++)
        {
            if (electrodeButtons[i]->getToggleState())
            {
                electrodeButtons[i]->setChannelNum (channel);
                electrodeButtons[i]->repaint();

				SpikeDetectorDynamic* processor = (SpikeDetectorDynamic*)getProcessor();
                processor->setChannel(electrodeList->getSelectedItemIndex(),
                                      i,
                                      channel - 1);
            }
        }
    }
}

void SpikeDetectorDynamicEditor::refreshElectrodeList()
{
    electrodeList->clear();

	SpikeDetectorDynamic* processor = (SpikeDetectorDynamic*)getProcessor();
    StringArray electrodeNames = processor->getElectrodeNames();

    for (int i = 0; i < electrodeNames.size(); i++)
    {
        electrodeList->addItem(electrodeNames[i], electrodeList->getNumItems()+1);
    }

    if (electrodeList->getNumItems() > 0)
    {
        electrodeList->setSelectedId(electrodeList->getNumItems(), sendNotification);
        electrodeList->setText(electrodeList->getItemText(electrodeList->getNumItems()-1));
        lastId = electrodeList->getNumItems();
        electrodeList->setEditableText(true);

        drawElectrodeButtons(electrodeList->getNumItems()-1);
    }
}

bool SpikeDetectorDynamicEditor::addElectrode(int nChans, int electrodeID)
{
	SpikeDetectorDynamic* processor = (SpikeDetectorDynamic*)getProcessor();
    if (processor->addElectrode(nChans, electrodeID))
    {
        refreshElectrodeList();
        return true;
    }
    else
    {
        return false;
    }
}

void SpikeDetectorDynamicEditor::removeElectrode(int index)
{
    std::cout << "Deleting electrode number " << index << std::endl;
	SpikeDetectorDynamic* processor = (SpikeDetectorDynamic*)getProcessor();
    processor->removeElectrode(index);
    refreshElectrodeList();

    int newIndex = jmin(index, electrodeList->getNumItems()-1);
    newIndex = jmax(newIndex, 0);

    electrodeList->setSelectedId(newIndex, sendNotification);
    electrodeList->setText(electrodeList->getItemText(newIndex));

    if (electrodeList->getNumItems() == 0)
    {
        electrodeButtons.clear();
        electrodeList->setEditableText(false);
    }
}

void SpikeDetectorDynamicEditor::labelTextChanged(Label* label)
{
    if (label->getText().equalsIgnoreCase("1") && isPlural)
    {
        for (int n = 1; n < electrodeTypes->getNumItems()+1; n++)
        {
            electrodeTypes->changeItemText(n,electrodeTypes->getItemText(n-1).trimCharactersAtEnd("s"));
        }

        isPlural = false;

        String currentText = electrodeTypes->getText();
        electrodeTypes->setText(currentText.trimCharactersAtEnd("s"));

    }
    else if (!label->getText().equalsIgnoreCase("1") && !isPlural)
    {
        for (int n = 1; n < electrodeTypes->getNumItems()+1; n++)
        {
            String currentString = electrodeTypes->getItemText(n-1);
            currentString += "s";

            electrodeTypes->changeItemText(n,currentString);
        }
        isPlural = true;

        String currentText = electrodeTypes->getText();
        electrodeTypes->setText(currentText += "s");
    }

	CoreServices::updateSignalChain(this);
}

void SpikeDetectorDynamicEditor::comboBoxChanged(ComboBox* comboBox)
{
    SpikeDetectorDynamic* processor = (SpikeDetectorDynamic*)getProcessor();

    if (comboBox == electrodeList)
    {
        int ID = comboBox->getSelectedId();

        if (ID == 0)
        {
            refreshElectrodeList();
        }
        else
        {
            lastId = ID;

            SimpleElectrode* e = processor->setCurrentElectrodeIndex(ID-1);
            electrodeEditorButtons[1]->setToggleState(e->isMonitored, dontSendNotification);
            drawElectrodeButtons(ID-1);
        }
    }
    else if (comboBox == detectionMethod) {
        // selecting which detection method to use
        int index = comboBox->getSelectedId(); //0 means no selection
        String method = comboBox->getItemText(index-1);
        processor->setDetectionMethod(method);
    }
    else if (comboBox == detectionSign) {
        int index = comboBox->getSelectedId();
        String dSign = comboBox->getItemText(index - 1);
        processor->setDetectionSign(dSign);
    }


    thresholdSlider->setActive(false);
}

void SpikeDetectorDynamicEditor::checkSettings()
{
    electrodeList->setSelectedId(0);
    drawElectrodeButtons(0);

	CoreServices::updateSignalChain(this);
	CoreServices::highlightEditor(this);
}

void SpikeDetectorDynamicEditor::drawElectrodeButtons(int ID)
{
	SpikeDetectorDynamic* processor = (SpikeDetectorDynamic*)getProcessor();
    electrodeButtons.clear();

    int width = 20;
    int height = 15;

    int numChannels = processor->getNumChannels(ID);
    int row = 0;
    int column = 0;

    Array<int> activeChannels;
    Array<double> thresholds;

    for (int i = 0; i < numChannels; i++)
    {
        ElectrodeButton* button = new ElectrodeButton(processor->getChannel(ID,i)+1);
        electrodeButtons.add(button);

        thresholds.add(processor->getChannelThreshold(ID,i));

        if (electrodeEditorButtons[0]->getToggleState())
        {
            button->setToggleState(false, dontSendNotification);
            button->setRadioGroupId(299);
        }
        else
        {
            activeChannels.add(processor->getChannel(ID,i));
            button->setToggleState(processor->isChannelActive(ID,i), dontSendNotification);
        }

        if (numChannels < 3)
            button->setBounds(145+(column++)*width, 78+row*height, width, 15);
        else
            button->setBounds(145+(column++)*width, 70+row*height, width, 15);

        addAndMakeVisible(button);
        button->addListener(this);

        if (column%2 == 0)
        {
            column = 0;
            row++;
        }
    }

    channelSelector->setActiveChannels(activeChannels);
    thresholdSlider->setValues(thresholds);
}
