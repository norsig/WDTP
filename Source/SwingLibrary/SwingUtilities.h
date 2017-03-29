/*
  ==============================================================================

    SwingUtilities.h
    Created: 6 Sep 2016 10:53:31am
    Author:  SwingCoder

  ==============================================================================
*/

#ifndef SWINGUTILITIES_H_INCLUDED
#define SWINGUTILITIES_H_INCLUDED

//=========================================================================
// Macros...

/** Display a messageBox to indicate some code/func hasn't been completed.
    Note: Arg 'info' must be a String. */
#ifndef NEED_TO_DO
#define NEED_TO_DO(info) AlertWindow::showMessageBox (AlertWindow::InfoIcon, \
                         String (info), String (__FILE__) + newLine + \
                         "Line: " + String (__LINE__))
#endif

/** Replace AlertWindow::showMessageWindow() */
#ifndef SHOW_MESSAGE
    //#define SHOW_MESSAGE(info) AlertWindow::showMessageBox (AlertWindow::InfoIcon, TRANS("Message"), String(info))
#define SHOW_MESSAGE(info) SplashWithMessage::showMessage (info) 
#endif

/** Use for replace the JUCE DBG(), it'll show the file and line-number in console.
    Note: this macro also run in Release mode. */
#ifndef DBGX
#define DBGX(info) JUCE_BLOCK_WITH_FORCED_SEMICOLON (juce::String tempDbgBuf; \
                   tempDbgBuf << info << " -> " << String (__FILE__) << \
                   " - Line: " << String (__LINE__); \
                   juce::Logger::outputDebugString (tempDbgBuf);)
#endif

//=========================================================================
/** Some static methods and const value */
struct SwingUtilities
{
    /** Return the platform-special font size */
    static const float getFontSize();

    /** Return the platform-special font name */
    static const String getFontName();

    /** Display "About window" which could be used on Android platform.
        Arg 'shortDescription' (may app's name): the first text line inside the box with
        brown colour and bigger size. */
    static void showAbout (const String& shortDescription,
                           const String& copyrightYear);

    /** This method will change the default WebBroswerCompoennt's core on Windows.

        By default, JUCE using IE-7 web-core which cannot load any js script when
        using WebBroswerComponent to display a html-page.

        After called, it will change the web-core to IE-10 on Windows (write to user's Windows register).
        This method should be called when the app is about to start (eg. first run it). */
    static void fixWindowsRegistry();

    //=========================================================================
    /** Write an ValueTree to a file. */
    static const bool writeValueTreeToFile (const ValueTree& treeToWrite,
                                            const File& fileToWriteTo,
                                            const bool usingGZipCompressData = false);

    /** Reads an ValueTree from the given file which was written by writeValueTreeToFile(). */
    static const ValueTree readValueTreeFromFile (const File& fileToReadFrom,
                                                  const bool usingGZipCompressData = false);

    /** get the arg-1 tree's all children number (include all the sub-children). the result is arg-2 */
    static void getAllChildrenNumbers (const ValueTree& tree, int& number);

    /** rename a file or dir */
    static bool renameFile (const File& file, const String& newName)
    {
        return file.moveFileTo (file.getSiblingFile (newName));
    }

    //=========================================================================
    /** Return such as "20150610112523"
        (year + month + day + hour + minute + second, without " " and "-") */
    static const String getTimeString (const Time& time);

    /** Return such as "20150610112523"
        (year + month + day + hour + minute + second, without " " and "-")

        plusDays: return currentTime + x days */
    static const String getCurrentTimeString (const int plusDays = 0);

    /** Convert a string such as "201506101125" to "2015.06.10 11:25:56" or "2015.06.10".

        The argument must be generated by getCurrentTimeString(),
        otherwise it will return a String::empty. */
    static const String getTimeStringWithSeparator (const String& dateAndTimeString,
                                                    bool includeTime);

    /** return: 2015-07-28 Mon 23:57:28.
        The argument must be generated by getCurrentTimeString().

        If the argument is empty, it will return String::empty. */
    static const String getDateWeekdayAndTimeString (const String& dateAndTimeString);

    /** Return 3 letters weekday name.
        The argument must be generated by getCurrentTimeString(). */
    static const String getWeekdayFromTimeString (const String& dateAndTimeString);

    /** if the argument valid and <= current time, then return true.

        The argument must be generated by getCurrentTimeString() and doesn't include any separator,
        eg. 20170308075525. otherwise it will return false.*/
    static const bool earlyThanCurrentTime (const String& dateAndTimeString);

    /** The Argument must be generated by getCurrentTimeString().

        if arg is empty, return today's Time. */
    static const Time getTimeFromString (const String& dateAndTimeString);

    /** if the given date and time string is valid, return true */
    static const bool isTimeStringValid (const String& dateAndTimeString);

    //=========================================================================
    /** Trim and replace all the invalid characters of the arg to '-',
        then return an valid file/dir name string. */
    static const String getValidFileName (const String& originalStr);

    /** provide 00:00.0 (min:sec:x) base on double seconds */
    static const String doubleToString (const double& seconds);

    /** convert the content of a Windows ANSI text file to UTF-8 string.

        NOTE 1: the arg should come from File::loadFileAsData().
        NOTE 2: if the file isn't ANSI format, the result will be messy code.
        NOTE 3: this method is only for Windows OS.
    */
    static const String convertANSIString (const File& ansiTextFile);

};

//=========================================================================
/** Could be used for single-line text input, such as search-input, title-input, etc.
    It will deliver the return-key event to its parent when user hit it.
*/
class InputTextEditor : public TextEditor
{
public:
    InputTextEditor (const String& componentName, juce_wchar passwordCharacter)
        : TextEditor (componentName, passwordCharacter)
    { }

    ~InputTextEditor() { }

    virtual bool keyPressed (const KeyPress& key) override
    {
        if (key == KeyPress::returnKey)
        {
            Component* parentComp = getParentComponent();

            if (parentComp != nullptr)
                parentComp->keyPressed (key);

            return true;
        }

        return TextEditor::keyPressed (key);
    }

private:
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (InputTextEditor)
};

//==============================================================================
/** This class is used for replace AlertWindow's modal run,
    since that method can't compile on Android.
    It could be used for register-window, complex-messageBox, etc.

    Usage: create object, add some widgets/components, set its size,
    then call showDialog().

    Note: this class only allows to have one custom component,
    and must set its size first when add a custom component.
*/
class SwingDialog : public Component,
                    private Button::Listener
{
public:
    SwingDialog (const String& dialogTitle);
    ~SwingDialog();

    void paint (Graphics&) override;
    void resized() override;

    /** set focus to the first text editor if there is one */
    void setFocus();

    void addButton (const String& btName, const KeyPress& shortKey);
    TextButton* getButton (const String& btName);

    //=========================================================================
    void addTextBlock (const String& content);
    void addTextEditor (const String& teName,
                        const String& content,
                        const String& labelTip = String::empty,
                        const bool isPw = false,
                        const bool readOnly = false);

    TextEditor* getTextEditor (const String& teName);
    const String getTextEditorContents (const String& editorName);

    //=========================================================================
    void addCustomComponent (Component* comp);

    /** Popup a modal dialog window to show */
    void showDialog (const String& windowTitle);

    virtual bool keyPressed (const KeyPress& key) override;

private:
    //=========================================================================
    virtual void buttonClicked (Button*) override;

    const Image logo;  // underwaySoft logo, size: 165 * 105

    ScopedPointer<Label> titleLb;
    OwnedArray<TextButton> bts;
    OwnedArray<Label> inputLbs;
    OwnedArray<InputTextEditor> inputs;
    OwnedArray<TextEditor> blocks;
    ScopedPointer<Component> custComp;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (SwingDialog)
};

//=========================================================================
/** Change the default backgroud colour to lightskyblue */
class StrechableBar : public StretchableLayoutResizerBar
{
public:
    StrechableBar (StretchableLayoutManager *layoutToUse,
                   int itemIndexInLayout,
                   bool isBarVertical)
        : StretchableLayoutResizerBar (layoutToUse, itemIndexInLayout, isBarVertical)
    {

    }

    //=========================================================================
    void paint (Graphics& g) override
    {
        g.setColour (isMouseOver() ? Colours::lightskyblue : Colour (0x00));
        g.fillAll();
    }

    //=========================================================================
    virtual void mouseEnter (const MouseEvent&) override
    {
        repaint();
    }

    virtual void mouseExit (const MouseEvent&) override
    {
        repaint();
    }

    //=========================================================================
private:

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (StrechableBar)
};

//=========================================================================
/** ColourSelector with preset 2 lines of colour-blocks */
class ColourSelectorWithPreset : public ColourSelector
{
public:
    ColourSelectorWithPreset();

    int getNumSwatches() const override                 { return colours.size(); }
    Colour getSwatchColour (int index) const override   { return colours[index]; }

    void setSwatchColour (int index, const Colour& newColour) const override;

private:
    //=================================================================================================
    static Array<Colour> colours;
    static void setPresetDefaultColour();

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (ColourSelectorWithPreset)
};

//=========================================================================
/** this class could replace messagebox (user no need to click any button to close it)
    it could run on Android. */
class SplashWithMessage : public SplashScreen
{
public:
    SplashWithMessage (const String& message) : SplashScreen ("Splash", 280, 60, true)
    {
        addAndMakeVisible (label = new Label());

        label->setFont (SwingUtilities::getFontSize() - 1.0f);
        label->setColour (Label::textColourId, Colours::whitesmoke.withAlpha (0.85f));
        label->setJustificationType (Justification::centred);
        label->setBounds (5, 5, getWidth() - 10, getHeight() - 10);
        label->setText (message, dontSendNotification);
    }

    void paint (Graphics& g)
    {
        g.setColour (Colours::black.withAlpha (0.75f));
        g.fillRoundedRectangle (getLocalBounds().toFloat(), 5.f);

        g.setColour (Colours::lightgrey);
        g.drawRoundedRectangle (getLocalBounds().toFloat(), 5.f, 1.f);
    }

    static void showMessage (const String& message)
    {
        SplashWithMessage* splash = new SplashWithMessage (message);
        LookAndFeel::getDefaultLookAndFeel().playAlertSound();

        splash->deleteAfterDelay (RelativeTime::seconds (3.0), true);
    }

private:
    //==============================================================================
    ScopedPointer<Label> label;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (SplashWithMessage)
};


//=================================================================================================
#endif  // SWINGUTILITIES_H_INCLUDED
