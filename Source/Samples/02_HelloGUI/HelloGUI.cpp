//
// Copyright (c) 2008-2017 the Urho3D project.
//
// Permission is hereby granted, free of charge, to any person obtaining a copy
// of this software and associated documentation files (the "Software"), to deal
// in the Software without restriction, including without limitation the rights
// to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
// copies of the Software, and to permit persons to whom the Software is
// furnished to do so, subject to the following conditions:
//
// The above copyright notice and this permission notice shall be included in
// all copies or substantial portions of the Software.
//
// THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
// IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
// FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
// AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
// LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
// OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
// THE SOFTWARE.
//

#include <Urho3D/Core/CoreEvents.h>
#include <Urho3D/Engine/Engine.h>
#include <Urho3D/Graphics/Graphics.h>
#include <Urho3D/Graphics/Texture2D.h>
#include <Urho3D/Input/Input.h>
#include <Urho3D/Resource/ResourceCache.h>
#include <Urho3D/UI/Button.h>
#include <Urho3D/UI/CheckBox.h>
#include <Urho3D/UI/LineEdit.h>
#include <Urho3D/UI/Text.h>
#include <Urho3D/UI/ToolTip.h>
#include <Urho3D/UI/UI.h>
#include <Urho3D/UI/UIEvents.h>
#include <Urho3D/UI/Window.h>
#include <Urho3D/UI/ScrollBar.h>
#include <Urho3D/UI/ScrollView.h>
#include <Urho3D/UI/Slider.h>
#include <Urho3D/UI/Font.h>
#include "HelloGUI.h"

#include <thread>
#include <Urho3D/DebugNew.h>

#include "ChineseString.h"

class NumberEdit : public LineEdit
{
  URHO3D_OBJECT(NumberEdit, LineEdit);
public:
  NumberEdit(Context* context)
    :LineEdit(context)
  {
    SubscribeToEvent(this, E_TEXTFINISHED, URHO3D_HANDLER(NumberEdit, Clamp));
  }

  void SetMinMaxValue(int minValue, int maxValue)
  {
    minValue_ = minValue;
    maxValue_ = maxValue;

    String newText = GetText();
    auto data = newText.CString();
    int test = atoi(data);

    if (test < minValue_ || test > maxValue_)
    {
      SetText(String(minValue_));
    }
  }

  static void RegisterObject(Context* context)
  {
    context->RegisterFactory<NumberEdit>("UI");

    URHO3D_COPY_BASE_ATTRIBUTES(LineEdit);
  }

  virtual void OnTextInput(const String& text)
  {
    if (text.Length() == 1)
    {
      char t = text[0];

      if (t >= '0' && t <= '9')
      {
        LineEdit::OnTextInput(text);
      }
    }
  }
 
private:

  void Clamp(StringHash eventType, VariantMap& eventData)
  {
    String newText = GetText();
    auto len = newText.Length();

    unsigned int i = 0;

    bool clamped = false;
    while (i < len && newText[i] == '0')
    {
      i++;
      clamped = true;
    }
    if(clamped)
      newText = newText.Substring(i);
    auto data = newText.CString();
    int test = atoi(data);

    if (test < minValue_)
    {
      SetText(String(minValue_));
    }
    else if (test > maxValue_)
    {
      SetText(String(maxValue_));
    }
    else if (clamped)
    {
      SetText(newText);
    }

  }

  int minValue_;
  int maxValue_;
};

URHO3D_DEFINE_APPLICATION_MAIN(HelloGUI)

HelloGUI::HelloGUI(Context* context) :
    Sample(context),
    uiRoot_(GetSubsystem<UI>()->GetRoot()),
    dragBeginPosition_(IntVector2::ZERO)
{
  NumberEdit::RegisterObject(context);
}

void HelloGUI::Start()
{
    // Execute base class startup
    Sample::Start();

    // Enable OS cursor
    GetSubsystem<Input>()->SetMouseVisible(true);

    // Load XML file containing default UI style sheet
    ResourceCache* cache = GetSubsystem<ResourceCache>();
    XMLFile* style = cache->GetResource<XMLFile>("UI/DefaultStyle.xml");

    // Set the loaded style as default style
    uiRoot_->SetDefaultStyle(style);

    // Initialize Window
    InitWindow();

    // Create and add some controls to the Window
    InitControls();

    
    InitMainUI();
    // Set the mouse mode to use in the sample
    InitFineTuning();

	InitStartPage();
	InitLoadingPage();
    Sample::InitMouseMode(MM_FREE);
}

void HelloGUI::InitControls()
{
    ResourceCache* cache = GetSubsystem<ResourceCache>();

    //PreLoad Chinese Character
    auto font = cache->GetResource<Font>("Fonts/pingfang.otf");
    font->GetFace(16.0);
    font->GetFace(12.0);
    font->GetFace(10.0);

    loginelement = new UIElement(context_);
    uiRoot_->AddChild(loginelement);

    LineEdit* lineEdit = new LineEdit(context_);
    lineEdit->SetName("Name");
    lineEdit->SetFixedSize(421.0f, 60.0f);
    lineEdit->SetPosition(430.0f, 305.0f);
	lineEdit->SetText("Name");
    lineEdit->SetClipBorder(IntRect(18, 0, 0, 0));

    LineEdit* linePwd = new LineEdit(context_);
    linePwd->SetName("Pwd");
    linePwd->SetFixedSize(421.0f, 60.0f);
    linePwd->SetPosition(430.0f, 395.0f);
    linePwd->SetText("Password");
    linePwd->SetClipBorder(IntRect(18, 0, 0, 0));

    // Logo
    BorderImage* logo = new BorderImage(context_);
    logo->SetName("logo");
    logo->SetPosition(582, 179);
    logo->SetFixedSize(118, 90);
    logo->SetTexture(cache->GetResource<Texture2D>("Textures/logo1.png"));
    logo->SetBlendMode(BLEND_ADD);

    // Create a Button
    Button* buttonLogin = new Button(context_);
    buttonLogin->SetName("buttonLogin");
    buttonLogin->SetPosition(430, 485);
    buttonLogin->SetFixedSize(190, 60);

    Text* login = new Text(context_);
    login->SetName("Login");
    login->SetPosition(498.0f, 501.0f);
    login->SetText("Login");

    Button* buttonCancel = new Button(context_);
    buttonCancel->SetName("buttonCancel");
    buttonCancel->SetPosition(661.0f, 485.0f);
    buttonCancel->SetFixedSize(190.0f, 60.0f);

    Text* cancel = new Text(context_);
    cancel->SetName("Cancel");
    cancel->SetPosition(722.0f, 501.0f);
    cancel->SetText("Cancel");

    loginelement->AddChild(buttonLogin);
    loginelement->AddChild(buttonCancel);
    loginelement->AddChild(lineEdit);
    loginelement->AddChild(linePwd);
    loginelement->AddChild(logo);
    loginelement->AddChild(login);
    loginelement->AddChild(cancel);

    lineEdit->SetStyleAuto();
    linePwd->SetStyleAuto();
    lineEdit->SetBlendMode(BLEND_ADD);
    linePwd->SetBlendMode(BLEND_ADD);

    login->SetStyleAuto();
    login->SetFont(cache->GetResource<Font>("Fonts/pingfang.otf"), 16.0);
    login->SetColor(Color(1.0f, 1.0f, 1.0f));

    buttonLogin->SetStyleAuto();
    buttonLogin->SetOpacity(0.3f);
    buttonLogin->SetBlendMode(BLEND_ADD);

    buttonCancel->SetStyleAuto();
    buttonCancel->SetOpacity(0.3f);
    buttonCancel->SetBlendMode(BLEND_ADD);

    cancel->SetStyleAuto();
    cancel->SetFont(cache->GetResource<Font>("Fonts/pingfang.otf"), 16.0);
    cancel->SetColor(Color(1.0f, 1.0f, 1.0f));
    
    // Subscribe to buttonClose release (following a 'press') events
    SubscribeToEvent(buttonLogin, E_PRESSED, URHO3D_HANDLER(HelloGUI, HandleLoginClicked));
    SubscribeToEvent(buttonLogin, E_RELEASED, URHO3D_HANDLER(HelloGUI, HandleLoginReleased));
    SubscribeToEvent(buttonCancel, E_PRESSED, URHO3D_HANDLER(HelloGUI, HandleCancelClicked));
    SubscribeToEvent(buttonCancel, E_RELEASED, URHO3D_HANDLER(HelloGUI, HandleCancelReleased));

}

void HelloGUI::InitMainUI()
{
  ResourceCache* cache = GetSubsystem<ResourceCache>();
  mainelement = new UIElement(context_);

  Slider* slider = new Slider(context_);
  slider->SetName("Slider");
  slider->SetPosition(480.0f, 763.0f);
  slider->SetFixedSize(321.0f, 2.0f);
  slider->SetRange(10.0f);
  slider->SetValue(5.0f);
  slider->SetStyleAuto();

  Button* buttonEditbg = new Button(context_);
  buttonEditbg->SetName("buttonEditBG");
  buttonEditbg->SetPosition(20.0f, 20.0f);
  buttonEditbg->SetFixedSize(60.0f, 60.0f);
  buttonEditbg->SetTexture(cache->GetResource<Texture2D>("Textures/icon/bg.png"));
  buttonEditbg->SetBlendMode(BLEND_ALPHA);

  Button* buttonMeasurebg = new Button(context_);
  buttonMeasurebg->SetName("buttonMeasureBG");
  buttonMeasurebg->SetPosition(1120.0f, 20.0f);
  buttonMeasurebg->SetFixedSize(60.0f, 60.0f);
  buttonMeasurebg->SetTexture(cache->GetResource<Texture2D>("Textures/icon/bg.png"));
  buttonMeasurebg->SetBlendMode(BLEND_ALPHA);

  Button* buttonCamerabg = new Button(context_);
  buttonCamerabg->SetName("buttonCameraBG");
  buttonCamerabg->SetPosition(1200.0f, 20.0f);
  buttonCamerabg->SetFixedSize(60.0f, 60.0f);
  buttonCamerabg->SetTexture(cache->GetResource<Texture2D>("Textures/icon/bg.png"));
  buttonCamerabg->SetBlendMode(BLEND_ALPHA);

  Button* buttonCubebg = new Button(context_);
  buttonCubebg->SetName("buttonCubeBG");
  buttonCubebg->SetPosition(1201.0f, 720.0f);
  buttonCubebg->SetFixedSize(60.0f, 60.0f);
  buttonCubebg->SetTexture(cache->GetResource<Texture2D>("Textures/icon/bg.png"));
  buttonCubebg->SetBlendMode(BLEND_ALPHA);

  BorderImage* buttonEdit = new BorderImage(context_);
  buttonEdit->SetName("buttonEdit");
  buttonEdit->SetPosition(35.0f, 38.0f);
  buttonEdit->SetFixedSize(30.0f, 24.0f);
  buttonEdit->SetTexture(cache->GetResource<Texture2D>("Textures/icon/content.png"));
  buttonEdit->SetBlendMode(BLEND_ADD);

  BorderImage* buttonMeasure = new BorderImage(context_);
  buttonMeasure->SetName("buttonMeasure");
  buttonMeasure->SetPosition(1135.0f, 35.0f);
  buttonMeasure->SetFixedSize(30.0f, 30.0f);
  buttonMeasure->SetTexture(cache->GetResource<Texture2D>("Textures/icon/measure.png"));
  buttonMeasure->SetBlendMode(BLEND_ADD);

  BorderImage* buttonCamera = new BorderImage(context_);
  buttonCamera->SetName("buttonCamera");
  buttonCamera->SetPosition(1215.0f, 38.0f);
  buttonCamera->SetFixedSize(30.0f, 24.0f);
  buttonCamera->SetTexture(cache->GetResource<Texture2D>("Textures/icon/photo.png"));
  buttonCamera->SetBlendMode(BLEND_ADD);

  BorderImage* buttonCube = new BorderImage(context_);
  buttonCube->SetName("buttonCube");
  buttonCube->SetPosition(1216.0f, 735.0f);
  buttonCube->SetFixedSize(30.0f, 30.0f);
  buttonCube->SetTexture(cache->GetResource<Texture2D>("Textures/icon/cube.png"));
  buttonCube->SetBlendMode(BLEND_ADD);
  
  /*Text* title = new Text(context_);
  title->SetName("Niceyou2");
  title->SetPosition(580.0f, 402.0f);
  title->SetText("NICE YOU");
  title->SetStyleAuto();
  title->SetFont(cache->GetResource<Font>("Fonts/monotype.ttf"), 20.0);
  title->SetColor(Color(1.0f, 0.0f, 0.572f));*/

  mainelement->AddChild(buttonEditbg);
  mainelement->AddChild(buttonMeasurebg);
  mainelement->AddChild(buttonCamerabg);
  mainelement->AddChild(buttonCubebg);
  mainelement->AddChild(buttonEdit);
  mainelement->AddChild(buttonMeasure);
  mainelement->AddChild(buttonCamera);
  mainelement->AddChild(buttonCube);
  //mainelement->AddChild(logo);

  SubscribeToEvent(buttonEditbg, E_RELEASED, URHO3D_HANDLER(HelloGUI, HandleEditReleased));
}

void HelloGUI::InitFineTuning()
{
  ResourceCache* cache = GetSubsystem<ResourceCache>();
  windowfinetuning_ = new Window(context_);
  windowfinetuning_->SetFixedSize(314.0f, 584.0f);
  windowfinetuning_->SetName("Window");
  windowfinetuning_->SetStyleAuto();
  windowfinetuning_->SetColor(Color(0.0, 0.0, 0.0, 0.3));
  windowfinetuning_->SetPosition(968.0f, 106.0f);
  windowfinetuning_->SetBlendMode(BLEND_ADD);
  
  for (int i = 0; i < 7; i++)
  {
    Button* buttonImage = new Button(context_);
    buttonImage->SetName("buttonABC" + i);
    buttonImage->SetStyle("ButtonABC");
    buttonImage->SetPosition(22.0f + 40.0f * i, 30.0f);
    buttonImage->SetFixedSize(30.0f, 30.0f);
    windowfinetuning_->AddChild(buttonImage);
  }

  for (int i = 0; i < 6; i++)
  {
    Text* basicSetting = new Text(context_);
    basicSetting->SetName("basicSetting" + i);
    basicSetting->SetPosition(30.0f, 80.0f + 82.0f * i);
    basicSetting->SetStyle("SmallerText");
    basicSetting->SetText(str_basicsetting);
    windowfinetuning_->AddChild(basicSetting);
  }
  
{
  Text* A = new Text(context_);
  A->SetName("A");
  A->SetPosition(31.0f, 34.0f);
  A->SetStyle("SmallText");
  A->SetText("A");
  A->SetPriority(1);
  Text* B = new Text(context_);
  B->SetName("B");
  B->SetPosition(71.0f, 34.0f);
  B->SetStyle("SmallText");
  B->SetText("B");
  B->SetPriority(1);
  Text* C = new Text(context_);
  C->SetName("C");
  C->SetPosition(111.0f, 34.0f);
  C->SetStyle("SmallText");
  C->SetText("C");
  C->SetPriority(1);
  Text* D = new Text(context_);
  D->SetName("D");
  D->SetPosition(151.0f, 34.0f);
  D->SetStyle("SmallText");
  D->SetText("D");
  D->SetPriority(1);
  Text* E = new Text(context_);
  E->SetName("E");
  E->SetPosition(191.0f, 34.0f);
  E->SetStyle("SmallText");
  E->SetText("E");
  E->SetPriority(1);
  Text* F = new Text(context_);
  F->SetName("F");
  F->SetPosition(231.0f, 34.0f);
  F->SetStyle("SmallText");
  F->SetText("F");
  F->SetPriority(1);
  Text* G = new Text(context_);
  G->SetName("G");
  G->SetPosition(271.0f, 34.0f);
  G->SetStyle("SmallText");
  G->SetText("G");
  G->SetPriority(1);
  windowfinetuning_->AddChild(A);
  windowfinetuning_->AddChild(B);
  windowfinetuning_->AddChild(C);
  windowfinetuning_->AddChild(D);
  windowfinetuning_->AddChild(E);
  windowfinetuning_->AddChild(F);
  windowfinetuning_->AddChild(G);

}

  for (int i = 0; i < 6; i++)
  {
    NumberEdit* settings = new NumberEdit(context_);
    settings->SetName("settings" + i);
    settings->SetStyle("LineEditSquare");
    settings->SetFixedSize(34.0f, 34.0f);
    settings->SetPosition(250.0f, 108.0f + 82.0f * i);
    settings->SetText("10");
    settings->SetClipBorder(IntRect(9, 0, 0, 0));
    settings->SetMinMaxValue(50, 80);

    editors_[i] = settings;
    windowfinetuning_->AddChild(settings);
  }

  for (int i = 0; i < 6; i++)
  {
    //FineTuningSlider
    Slider* sliderSetting = new Slider(context_);
    sliderSetting->SetName("sliderSetting" + i);
    sliderSetting->SetPosition(30.0f, 122.0f + 82.0f * i);
    sliderSetting->SetFixedSize(200, 30);
    sliderSetting->GetKnob()->SetFixedSize(30, 30);
    sliderSetting->SetRange(100.0);

    sliderSetting->SetValue(5.0f);
    sliderSetting->SetStyle("FineTuningSlider");
    sliderSetting->SetBlendMode(BlendMode::BLEND_ALPHA);
    sliders_[i] = sliderSetting;
    windowfinetuning_->AddChild(sliderSetting);
  }

  
  SubscribeToEvent(sliders_[0], E_SLIDERCHANGED, URHO3D_HANDLER(HelloGUI, HandleSliderChanged0));
  SubscribeToEvent(sliders_[1], E_SLIDERCHANGED, URHO3D_HANDLER(HelloGUI, HandleSliderChanged1));
  SubscribeToEvent(sliders_[2], E_SLIDERCHANGED, URHO3D_HANDLER(HelloGUI, HandleSliderChanged2));
  SubscribeToEvent(sliders_[3], E_SLIDERCHANGED, URHO3D_HANDLER(HelloGUI, HandleSliderChanged3));
  SubscribeToEvent(sliders_[4], E_SLIDERCHANGED, URHO3D_HANDLER(HelloGUI, HandleSliderChanged4));
  SubscribeToEvent(sliders_[5], E_SLIDERCHANGED, URHO3D_HANDLER(HelloGUI, HandleSliderChanged5));
  SubscribeToEvent(editors_[0], E_TEXTCHANGED, URHO3D_HANDLER(HelloGUI, HandleEditorChanged0));
  SubscribeToEvent(editors_[1], E_TEXTCHANGED, URHO3D_HANDLER(HelloGUI, HandleEditorChanged1));
  SubscribeToEvent(editors_[2], E_TEXTCHANGED, URHO3D_HANDLER(HelloGUI, HandleEditorChanged2));
  SubscribeToEvent(editors_[3], E_TEXTCHANGED, URHO3D_HANDLER(HelloGUI, HandleEditorChanged3));
  SubscribeToEvent(editors_[4], E_TEXTCHANGED, URHO3D_HANDLER(HelloGUI, HandleEditorChanged4));
  SubscribeToEvent(editors_[5], E_TEXTCHANGED, URHO3D_HANDLER(HelloGUI, HandleEditorChanged5));
}

void HelloGUI::InitStartPage()
{
	ResourceCache* cache = GetSubsystem<ResourceCache>();
	startPage_ = new UIElement(context_);
	startPage_->SetPosition(0, 0);
	Button* circle = new Button(context_);
	circle->SetName("Circle");
	circle->SetPosition(512, 272);
	circle->SetFixedSize(256, 256);
	circle->SetOpacity(0.8);

	Text* startText = new Text(context_);
	startText->SetPosition(597, 372);
	startText->SetColor(Color(1.0F, 0.0F, 0.57F)); //#FF0092
	startText->SetText("Start");
	startText->SetFont(cache->GetResource<Font>("Fonts/pingfang.otf"), 36);
	
	startPage_->AddChild(circle);
	startPage_->AddChild(startText);
	startPage_->SetVisible(false);
	uiRoot_->AddChild(startPage_);
	
	circle->SetStyle("Circle");

	SubscribeToEvent(circle, E_RELEASED, URHO3D_HANDLER(HelloGUI, HandleShowLoadingPage));
}

void HelloGUI::InitLoadingPage()
{
	ResourceCache* cache = GetSubsystem<ResourceCache>();
	loadingPage_ = new UIElement(context_);
	loadingPage_->SetPosition(0, 0);

	BorderImage* circle = new BorderImage(context_);
	circle->SetName("Circle");
	circle->SetPosition(512, 272);
	circle->SetFixedSize(256, 256);
	circle->SetOpacity(0.8F);

	animateCircle_ = new BorderImage(context_);
	animateCircle_->SetName("AnimateCircle");
	animateCircle_->SetPosition(632, 264);
	animateCircle_->SetFixedSize(16, 16);
	animateCircle_->SetColor(Color(1.0F, 0.0F, 0.57F));
	animateCircle_->SetPriority(1);

	BorderImage* logo = new BorderImage(context_);
	logo->SetName("logo");
	logo->SetPosition(603, 371);
	logo->SetFixedSize(74, 59);
	logo->SetTexture(cache->GetResource<Texture2D>("Textures/logo1.png"));
	logo->SetBlendMode(BLEND_ALPHA);

	loadingPage_->AddChild(animateCircle_);
	loadingPage_->AddChild(circle);
	loadingPage_->AddChild(logo);
	loadingPage_->SetVisible(false);
	uiRoot_->AddChild(loadingPage_);

	circle->SetStyle("Circle");
	animateCircle_->SetStyle("Circle");

}

void HelloGUI::InitWindow()
{ 
  ResourceCache* cache = GetSubsystem<ResourceCache>();
  window_ = new Window(context_);
  window_->SetFixedSize(320.0f, 800.0f);
  window_->SetName("Window");
  window_->SetStyleAuto();
  window_->SetColor(Color(0.0,0.0,0.0,0.3));
  window_->SetBlendMode(BLEND_ADD);

  Text* title = new Text(context_);
  title->SetName("ChooseType");
  title->SetPosition(67.0f, 30.0f);
  title->SetText(str_selection_type);
  title->SetStyleAuto();
  
  Button* buttonCloseWindow = new Button(context_);
  buttonCloseWindow->SetName("buttonCloseWindow");
  buttonCloseWindow->SetPosition(266.0f, 30.0f);
  buttonCloseWindow->SetFixedSize(24.0f, 24.0f);
  buttonCloseWindow->SetTexture(cache->GetResource<Texture2D>("Textures/icon/photo.png"));
  buttonCloseWindow->SetBlendMode(BLEND_ADD);

  ScrollView* scrollView = new ScrollView(context_);
  scrollView->SetName("SV");
  scrollView->SetPosition(0.0f, 90.0f);
  scrollView->SetFixedSize(312.0f, 618.0f);
  scrollView->SetStyleAuto();
  scrollView->GetScrollPanel()->SetColor(Color(0.0,0.0,0.0,1.0));
  scrollView->GetScrollPanel()->SetBlendMode(BLEND_ADD);

  scrollView->RemoveChild(scrollView->GetHorizontalScrollBar());
  auto scrollBar = scrollView->GetVerticalScrollBar();

  scrollBar->RemoveChild(scrollBar->GetBackButton());
  scrollBar->RemoveChild(scrollBar->GetForwardButton());
  
  SubscribeToEvent(scrollBar, E_RESIZED, URHO3D_HANDLER(HelloGUI, HandleResized));
  auto window2 = new Window(context_);
  window2->SetFixedSize(312.0f, 1000.0f);
  window2->SetName("Window2");
  window2->SetColor(Color(0.0, 0.0, 0.0, 1.0));
  window2->SetBlendMode(BLEND_ADD);

  for (int i = 0; i < 5; i++)
  {
    BorderImage* buttonImage = new Button(context_);
    buttonImage->SetFocusMode(FocusMode::FM_NOTFOCUSABLE);
    buttonImage->SetName("buttonImage" + i);
    buttonImage->SetPosition(31.0f, 0.0f + i * 141.0f);
    buttonImage->SetFixedSize(120.0f, 120.0f);
    buttonImage->SetTexture(cache->GetResource<Texture2D>("Textures/list_images1.jpg"));
    window2->AddChild(buttonImage);
  }
  
  for (int i = 0; i < 5; i++)
  {
    BorderImage* buttonImage = new Button(context_);
    buttonImage->SetFocusMode(FocusMode::FM_NOTFOCUSABLE);
    buttonImage->SetName("buttonImageright" + i);
    buttonImage->SetPosition(170.0f, 0.0f + i * 141.0f);
    buttonImage->SetFixedSize(120.0f, 120.0f);
    buttonImage->SetTexture(cache->GetResource<Texture2D>("Textures/background.jpg"));
    window2->AddChild(buttonImage);
  }

  scrollView->SetContentElement(window2);
  
  Button* buttonGenerate = new Button(context_);
  buttonGenerate->SetName("buttonGenerate");
  buttonGenerate->SetStyleAuto();
  buttonGenerate->SetPosition(30.0f, 729.0f);
  buttonGenerate->SetFixedSize(120.0f, 48.0f);
  
  Button* buttonPrint = new Button(context_);
  buttonPrint->SetName("buttonPrint");
  buttonPrint->SetStyleAuto();
  buttonPrint->SetPosition(170.0f, 729.0f);
  buttonPrint->SetFixedSize(120.0f, 48.0f);

  Text* generate = new Text(context_);
  generate->SetName("Generate");
  generate->SetPosition(57.0f, 741.0f);
  generate->SetText(str_generate);
  generate->SetStyleAuto();
  generate->SetStyle("SmallText");

  Text* print = new Text(context_);
  print->SetName("Print");
  print->SetPosition(214.0f, 741.0f);
  print->SetText(str_print);
  print->SetStyle("SmallText");

  window_->AddChild(scrollView);
  window_->AddChild(title);
  window_->AddChild(buttonCloseWindow);
  window_->AddChild(buttonGenerate);
  window_->AddChild(buttonPrint);
  window_->AddChild(generate);
  window_->AddChild(print);  
  SubscribeToEvent(buttonGenerate, E_RELEASED, URHO3D_HANDLER(HelloGUI, HandleGenerateReleased));
  
}

void HelloGUI::CreateDraggableFish()
{
    ResourceCache* cache = GetSubsystem<ResourceCache>();
    Graphics* graphics = GetSubsystem<Graphics>();

    // Create a draggable Fish button
    Button* draggableFish = new Button(context_);
    draggableFish->SetTexture(cache->GetResource<Texture2D>("Textures/UrhoDecal.dds")); // Set texture
    draggableFish->SetBlendMode(BLEND_ADD);
    draggableFish->SetSize(128, 128);
    draggableFish->SetPosition((graphics->GetWidth() - draggableFish->GetWidth()) / 2, 200);
    draggableFish->SetName("Fish");
    uiRoot_->AddChild(draggableFish);

    // Add a tooltip to Fish button
    ToolTip* toolTip = new ToolTip(context_);
    draggableFish->AddChild(toolTip);
    toolTip->SetPosition(IntVector2(draggableFish->GetWidth() + 5, draggableFish->GetWidth() / 2)); // slightly offset from close button
    BorderImage* textHolder = new BorderImage(context_);
    toolTip->AddChild(textHolder);
    textHolder->SetStyle("ToolTipBorderImage");
    Text* toolTipText = new Text(context_);
    textHolder->AddChild(toolTipText);
    toolTipText->SetStyle("ToolTipText");
    toolTipText->SetText("Please drag me!");

    // Subscribe draggableFish to Drag Events (in order to make it draggable)
    // See "Event list" in documentation's Main Page for reference on available Events and their eventData
    SubscribeToEvent(draggableFish, E_DRAGBEGIN, URHO3D_HANDLER(HelloGUI, HandleDragBegin));
    SubscribeToEvent(draggableFish, E_DRAGMOVE, URHO3D_HANDLER(HelloGUI, HandleDragMove));
    SubscribeToEvent(draggableFish, E_DRAGEND, URHO3D_HANDLER(HelloGUI, HandleDragEnd));
}

void HelloGUI::HandleDragBegin(StringHash eventType, VariantMap& eventData)
{
    // Get UIElement relative position where input (touch or click) occurred (top-left = IntVector2(0,0))
    dragBeginPosition_ = IntVector2(eventData["ElementX"].GetInt(), eventData["ElementY"].GetInt());
}

void HelloGUI::HandleDragMove(StringHash eventType, VariantMap& eventData)
{
    IntVector2 dragCurrentPosition = IntVector2(eventData["X"].GetInt(), eventData["Y"].GetInt());
    UIElement* draggedElement = static_cast<UIElement*>(eventData["Element"].GetPtr());
    draggedElement->SetPosition(dragCurrentPosition - dragBeginPosition_);
}

void HelloGUI::HandleDragEnd(StringHash eventType, VariantMap& eventData) // For reference (not used here)
{
}


void HelloGUI::HandleResized(StringHash eventType, VariantMap& eventData)
{
  auto scrollBar = (ScrollBar*)eventData[Positioned::P_ELEMENT].GetPtr();

  scrollBar->GetSlider()->SetHeight(scrollBar->GetHeight());
  scrollBar->GetSlider()->SetPosition(0, 0);
}

void HelloGUI::HandleClosePressed(StringHash eventType, VariantMap& eventData)
{
    if (GetPlatform() != "Web")
        engine_->Exit();
}

void HelloGUI::HandleLoginClicked(StringHash eventType, VariantMap& eventData)
{

}


void HelloGUI::HandleEditReleased(StringHash eventType, VariantMap& eventData)
{
  auto lb = static_cast<Button*>(mainelement->GetChild("buttonEdit", true));
  auto lb2 = static_cast<Button*>(mainelement->GetChild("buttonEditBG", true));
  mainelement->RemoveChild(lb);
  mainelement->RemoveChild(lb2);
  uiRoot_->AddChild(window_);
  //auto print = static_cast<Text*>(window_->GetChild("Print", true));
  //auto generate = static_cast<Text*>(window_->GetChild("Generate", true));
  auto buttonGenerate = static_cast<Button*>(window_->GetChild("buttonGenerate", true));
  auto buttonPrint = static_cast<Button*>(window_->GetChild("buttonPrint", true));

  //print->SetFontSize(12.0f);
  //generate->SetFontSize(12.0f);
  buttonGenerate->SetBlendMode(BLEND_ADD);
  buttonPrint->SetBlendMode(BLEND_ADD);
}

void HelloGUI::HandleGenerateReleased(StringHash eventType, VariantMap& eventData)
{
  uiRoot_->RemoveChild(uiRoot_->GetChild(("Window"),true));
  uiRoot_->AddChild(windowfinetuning_);
}

void HelloGUI::HandleSliderChanged0(StringHash eventType, VariantMap& eventData)
{
  if (currentSignal_ == 1)
  {
    currentSignal_ = -1;
    return;
  }

  currentSignal_ = 1;

  int sliderValue = eventData[SliderChanged::P_VALUE].Get<float>();
  editors_[0]->SetText(String(sliderValue));
}

void HelloGUI::HandleEditorChanged0(StringHash eventType, VariantMap& eventData)
{
  if (currentSignal_ == 1)
  {
    currentSignal_ = -1;
    return;
  }

  currentSignal_ = 1;
  const auto& editorValueStr = eventData[TextChanged::P_TEXT].ToString();
  int editorValue = atoi(editorValueStr.CString());
  sliders_[0]->SetValue(editorValue);
}

void HelloGUI::HandleSliderChanged1(StringHash eventType, VariantMap& eventData)
{
  if (currentSignal_ == 1)
  {
    currentSignal_ = -1;
    return;
  }

  currentSignal_ = 1;

  int sliderValue = eventData[SliderChanged::P_VALUE].Get<float>();
  editors_[1]->SetText(String(sliderValue));
}

void HelloGUI::HandleEditorChanged1(StringHash eventType, VariantMap& eventData)
{
  if (currentSignal_ == 1)
  {
    currentSignal_ = -1;
    return;
  }

  currentSignal_ = 1;
  const auto& editorValueStr = eventData[TextChanged::P_TEXT].ToString();
  int editorValue = atoi(editorValueStr.CString());
  sliders_[1]->SetValue(editorValue);
}

void HelloGUI::HandleSliderChanged2(StringHash eventType, VariantMap& eventData)
{
  if (currentSignal_ == 1)
  {
    currentSignal_ = -1;
    return;
  }

  currentSignal_ = 1;

  int sliderValue = eventData[SliderChanged::P_VALUE].Get<float>();
  editors_[2]->SetText(String(sliderValue));
}

void HelloGUI::HandleEditorChanged2(StringHash eventType, VariantMap& eventData)
{
  if (currentSignal_ == 1)
  {
    currentSignal_ = -1;
    return;
  }

  currentSignal_ = 1;
  const auto& editorValueStr = eventData[TextChanged::P_TEXT].ToString();
  int editorValue = atoi(editorValueStr.CString());
  sliders_[2]->SetValue(editorValue);
}

void HelloGUI::HandleSliderChanged3(StringHash eventType, VariantMap& eventData)
{
  if (currentSignal_ == 1)
  {
    currentSignal_ = -1;
    return;
  }

  currentSignal_ = 1;

  int sliderValue = eventData[SliderChanged::P_VALUE].Get<float>();
  editors_[3]->SetText(String(sliderValue));
}

void HelloGUI::HandleEditorChanged3(StringHash eventType, VariantMap& eventData)
{
  if (currentSignal_ == 1)
  {
    currentSignal_ = -1;
    return;
  }

  currentSignal_ = 1;
  const auto& editorValueStr = eventData[TextChanged::P_TEXT].ToString();
  int editorValue = atoi(editorValueStr.CString());
  sliders_[3]->SetValue(editorValue);
}

void HelloGUI::HandleSliderChanged4(StringHash eventType, VariantMap& eventData)
{
  if (currentSignal_ == 1)
  {
    currentSignal_ = -1;
    return;
  }

  currentSignal_ = 1;

  int sliderValue = eventData[SliderChanged::P_VALUE].Get<float>();
  editors_[4]->SetText(String(sliderValue));
}

void HelloGUI::HandleEditorChanged4(StringHash eventType, VariantMap& eventData)
{
  if (currentSignal_ == 1)
  {
    currentSignal_ = -1;
    return;
  }

  currentSignal_ = 1;
  const auto& editorValueStr = eventData[TextChanged::P_TEXT].ToString();
  int editorValue = atoi(editorValueStr.CString());
  sliders_[4]->SetValue(editorValue);
}

void HelloGUI::HandleSliderChanged5(StringHash eventType, VariantMap& eventData)
{
  if (currentSignal_ == 1)
  {
    currentSignal_ = -1;
    return;
  }

  currentSignal_ = 1;

  int sliderValue = eventData[SliderChanged::P_VALUE].Get<float>();
  editors_[5]->SetText(String(sliderValue));
}

void HelloGUI::HandleEditorChanged5(StringHash eventType, VariantMap& eventData)
{
  if (currentSignal_ == 1)
  {
    currentSignal_ = -1;
    return;
  }

  currentSignal_ = 1;
  const auto& editorValueStr = eventData[TextChanged::P_TEXT].ToString();
  int editorValue = atoi(editorValueStr.CString());
  sliders_[5]->SetValue(editorValue);
}

void HelloGUI::HandleShowLoadingPage(StringHash eventType, VariantMap & eventData)
{
	startPage_->SetVisible(false);
	loadingPage_->SetVisible(true);
	SubscribeToEvent(E_POSTUPDATE, URHO3D_HANDLER(HelloGUI, HandleLoadingCircleUpdate));
}

void HelloGUI::HandleLoadingCircleUpdate(StringHash eventType, VariantMap & eventData)
{
	float timeStep = eventData[PostUpdate::P_TIMESTEP].GetFloat();
	
	loadingDurationTime_ += timeStep;
	float alpha = loadingDurationTime_;

	// 640, 400
	int x = 128 * sin(alpha) + 640 - 8;
	int y = -128 * cos(alpha) + 400 - 8;
	animateCircle_->SetPosition(x, y);
}

void HelloGUI::HandleLoginReleased(StringHash eventType, VariantMap& eventData)
{
  loginelement->RemoveAllChildren();
  //uiRoot_->AddChild(mainelement);
  startPage_->SetVisible(true);
}

void HelloGUI::HandleCancelClicked(StringHash eventType, VariantMap& eventData)
{
  Button* buttonCancel = new Button(context_);
  buttonCancel->SetName("buttonCancel2");
  buttonCancel->SetPosition(661.0f, 485.0f);
  buttonCancel->SetFixedSize(190.0f, 60.0f);

  buttonCancel->SetColor(Color(1.0f, 0.0f, 0.572f));
  buttonCancel->SetBlendMode(BLEND_REPLACE);
  buttonCancel->SetOpacity(1.0);
  loginelement->InsertChild(0, buttonCancel);
}

void HelloGUI::HandleCancelReleased(StringHash eventType, VariantMap& eventData)
{
  auto lb = static_cast<Button*>(loginelement->GetChild("buttonCancel2", true));
  loginelement->RemoveChild(lb);
}
