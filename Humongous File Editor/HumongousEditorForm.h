#pragma once

#include <string>
#include <windows.h>

#include "ProgressBarForm.h"
#include "functions/FileIndexer.h"
#include "functions/FileDecrypter.h"
#include "functions/ResourceGatherer.h"
#include "forms/HumongousNode.h"
#include "forms/HumongousButton.h"
#include "functions/TabFunctions.h"
#include "file/Files.h"
#include <allocators/TempAllocator.h>

using namespace  System::Windows::Forms;

namespace HumongousFileEditor
{
	/// <summary>
	/// Summary for EditorForm
	/// </summary>
	public ref class HumongousEditorForm : public System::Windows::Forms::Form
	{
	public:
		HumongousEditorForm(void)
		{
			InitializeComponent();
			//
			//TODO: Add the constructor code here
			//
		}
		System::Windows::Forms::SplitContainer^ splitContainer;
		System::Windows::Forms::ToolStripMenuItem^ optionSeperator1;

		System::Windows::Forms::ToolStripMenuItem^ optionOpen;
		System::Windows::Forms::ToolStripMenuItem^ optionSaveAs;
		System::Windows::Forms::ToolStripSeparator^ toolStripSeparator2;
		System::Windows::Forms::ToolStripMenuItem^ optionDecrypt;
		System::Windows::Forms::ToolStripMenuItem^ optionIndex;
		System::Windows::Forms::ToolStripMenuItem^ optionQuit;
		System::Windows::Forms::ToolStripSeparator^ toolStripSeparator3;

		System::Windows::Forms::ToolStripMenuItem^ helpMenu;
		System::Windows::Forms::ToolStripMenuItem^ optionAbout;

		System::Windows::Forms::MenuStrip^ topMenu;



		System::Windows::Forms::TreeView^ entryView;


		System::Windows::Forms::ToolTip^ tooltipOpen = gcnew System::Windows::Forms::ToolTip();
		System::Windows::Forms::ToolTip^ tooltipSave = gcnew System::Windows::Forms::ToolTip();
		System::Windows::Forms::ToolTip^ tooltipSaveAs = gcnew System::Windows::Forms::ToolTip();
		System::Windows::Forms::ToolTip^ tooltipAbout = gcnew System::Windows::Forms::ToolTip();
		System::Windows::Forms::ToolTip^ tooltipDecrypt = gcnew System::Windows::Forms::ToolTip();
		System::Windows::Forms::ToolTip^ tooltipIndex = gcnew System::Windows::Forms::ToolTip();

		System::Windows::Forms::ToolStripMenuItem^ optionSave;
		System::Windows::Forms::ToolStripSeparator^ toolStripSeparator1;


		System::String^ windowTitle = gcnew System::String("Humongous File Editor");
    public: System::Windows::Forms::Button^ openButton;
    public: System::Windows::Forms::Button^ saveButton;
    public: System::Windows::Forms::Button^ aboutButton;
    public: System::Windows::Forms::Button^ saveAsButton;
	public: System::Windows::Forms::Button^ decryptButton;
	public: System::Windows::Forms::Button^ indexButton;
	public: System::Windows::Forms::Panel^ panel2;
    public: System::Windows::Forms::Panel^ panel3;
    public: System::Windows::Forms::Panel^ panel4;
	public: System::Windows::Forms::Panel^ actionPanel;

		  System::Windows::Forms::TabControl^ tabControl1;
	public: System::Windows::Forms::TabPage^ tabWelcome;
		  HumongousFileEditor::ProgressBarForm progressBar;
	protected:
		/// <summary>
		/// Clean up any resources being used.
		/// </summary>
		~HumongousEditorForm()
		{
			if (components)
			{
				delete components;
			}
		}

	private:
		/// <summary>
		/// Required designer variable.
		/// </summary>
		System::ComponentModel::Container^ components;

#pragma region Windows Form Designer generated code
		/// <summary>
		/// Required method for Designer support - do not modify
		/// the contents of this method with the code editor.
		/// </summary>
		void InitializeComponent(void)
		{
			System::ComponentModel::ComponentResourceManager^ resources = (gcnew System::ComponentModel::ComponentResourceManager(HumongousEditorForm::typeid));
			this->splitContainer = (gcnew System::Windows::Forms::SplitContainer());
			this->entryView = (gcnew System::Windows::Forms::TreeView());
			this->tabControl1 = (gcnew System::Windows::Forms::TabControl());
			this->optionSeperator1 = (gcnew System::Windows::Forms::ToolStripMenuItem());
			this->optionOpen = (gcnew System::Windows::Forms::ToolStripMenuItem());
			this->toolStripSeparator1 = (gcnew System::Windows::Forms::ToolStripSeparator());
			this->optionSave = (gcnew System::Windows::Forms::ToolStripMenuItem());
			this->optionSaveAs = (gcnew System::Windows::Forms::ToolStripMenuItem());
			this->toolStripSeparator2 = (gcnew System::Windows::Forms::ToolStripSeparator());
			this->optionIndex = (gcnew System::Windows::Forms::ToolStripMenuItem());
			this->optionDecrypt = (gcnew System::Windows::Forms::ToolStripMenuItem());
			this->toolStripSeparator3 = (gcnew System::Windows::Forms::ToolStripSeparator());
			this->optionQuit = (gcnew System::Windows::Forms::ToolStripMenuItem());
			this->helpMenu = (gcnew System::Windows::Forms::ToolStripMenuItem());
			this->optionAbout = (gcnew System::Windows::Forms::ToolStripMenuItem());
			this->topMenu = (gcnew System::Windows::Forms::MenuStrip());
			this->openButton = (gcnew System::Windows::Forms::Button());
			this->saveButton = (gcnew System::Windows::Forms::Button());
			this->aboutButton = (gcnew System::Windows::Forms::Button());
			this->panel2 = (gcnew System::Windows::Forms::Panel());
			this->saveAsButton = (gcnew System::Windows::Forms::Button());
			this->panel3 = (gcnew System::Windows::Forms::Panel());
			this->panel4 = (gcnew System::Windows::Forms::Panel());
			this->decryptButton = (gcnew System::Windows::Forms::Button());
			this->actionPanel = (gcnew System::Windows::Forms::Panel());
			this->indexButton = (gcnew System::Windows::Forms::Button());
			this->tabWelcome = (gcnew System::Windows::Forms::TabPage());
			(cli::safe_cast<System::ComponentModel::ISupportInitialize^>(this->splitContainer))->BeginInit();
			this->splitContainer->Panel1->SuspendLayout();
			this->splitContainer->Panel2->SuspendLayout();
			this->splitContainer->SuspendLayout();
			this->tabControl1->SuspendLayout();
			this->topMenu->SuspendLayout();
			this->actionPanel->SuspendLayout();
			this->SuspendLayout();
			// 
			// splitContainer
			// 
			this->splitContainer->Anchor = static_cast<System::Windows::Forms::AnchorStyles>((((System::Windows::Forms::AnchorStyles::Top | System::Windows::Forms::AnchorStyles::Bottom)
				| System::Windows::Forms::AnchorStyles::Left)
				| System::Windows::Forms::AnchorStyles::Right));
			this->splitContainer->BackColor = System::Drawing::SystemColors::ControlLight;
			this->splitContainer->Location = System::Drawing::Point(0, 79);
			this->splitContainer->Name = L"splitContainer";
			// 
			// splitContainer.Panel1
			// 
			this->splitContainer->Panel1->BackColor = System::Drawing::SystemColors::Window;
			this->splitContainer->Panel1->Controls->Add(this->entryView);
			this->splitContainer->Panel1->Padding = System::Windows::Forms::Padding(10);
			// 
			// splitContainer.Panel2
			// 
			this->splitContainer->Panel2->BackColor = System::Drawing::SystemColors::Window;
			this->splitContainer->Panel2->Controls->Add(this->tabControl1);
			this->splitContainer->Panel2->Margin = System::Windows::Forms::Padding(5);
			this->splitContainer->Panel2->Padding = System::Windows::Forms::Padding(10);
			this->splitContainer->Size = System::Drawing::Size(991, 466);
			this->splitContainer->SplitterDistance = 561;
			this->splitContainer->TabIndex = 1;
			// 
			// entryView
			// 
			this->entryView->Anchor = static_cast<System::Windows::Forms::AnchorStyles>((((System::Windows::Forms::AnchorStyles::Top | System::Windows::Forms::AnchorStyles::Bottom)
				| System::Windows::Forms::AnchorStyles::Left)
				| System::Windows::Forms::AnchorStyles::Right));
			this->entryView->BackColor = System::Drawing::Color::FromArgb(static_cast<System::Int32>(static_cast<System::Byte>(249)), static_cast<System::Int32>(static_cast<System::Byte>(249)),
				static_cast<System::Int32>(static_cast<System::Byte>(249)));
			this->entryView->ItemHeight = 20;
			this->entryView->Location = System::Drawing::Point(12, 13);
			this->entryView->Name = L"entryView";
			this->entryView->Size = System::Drawing::Size(536, 443);
			this->entryView->TabIndex = 0;
			this->entryView->MouseDoubleClick += gcnew System::Windows::Forms::MouseEventHandler(this, &HumongousEditorForm::entryView_DoubleClick);
			// 
			// tabControl1
			// 
			this->tabControl1->AllowDrop = true;
			this->tabControl1->Controls->Add(this->tabWelcome);
			this->tabControl1->Dock = System::Windows::Forms::DockStyle::Fill;
			this->tabControl1->DrawMode = System::Windows::Forms::TabDrawMode::OwnerDrawFixed;
			this->tabControl1->ImeMode = System::Windows::Forms::ImeMode::NoControl;
			this->tabControl1->Location = System::Drawing::Point(10, 10);
			this->tabControl1->Name = L"tabControl1";
			this->tabControl1->SelectedIndex = 0;
			this->tabControl1->Size = System::Drawing::Size(406, 446);
			this->tabControl1->TabIndex = 0;
			this->tabControl1->DrawItem += gcnew System::Windows::Forms::DrawItemEventHandler(this, &HumongousEditorForm::tabControl1_DrawX);
			this->tabControl1->MouseUp += gcnew System::Windows::Forms::MouseEventHandler(this, &HumongousEditorForm::tabControl1_MouseDown);
			// 
			// optionSeperator1
			// 
			this->optionSeperator1->DropDownItems->AddRange(gcnew cli::array< System::Windows::Forms::ToolStripItem^  >(9) {
				this->optionOpen,
					this->toolStripSeparator1, this->optionSave, this->optionSaveAs, this->toolStripSeparator2, this->optionIndex, this->optionDecrypt,
					this->toolStripSeparator3, this->optionQuit
			});
			this->optionSeperator1->Name = L"optionSeperator1";
			this->optionSeperator1->Size = System::Drawing::Size(54, 29);
			this->optionSeperator1->Text = L"File";
			// 
			// optionOpen
			// 
			this->optionOpen->Name = L"optionOpen";
			this->optionOpen->ShortcutKeys = static_cast<System::Windows::Forms::Keys>((System::Windows::Forms::Keys::Control | System::Windows::Forms::Keys::O));
			this->optionOpen->Size = System::Drawing::Size(285, 34);
			this->optionOpen->Text = L"Open";
			this->optionOpen->ToolTipText = L"Open file";
			this->optionOpen->Click += gcnew System::EventHandler(this, &HumongousEditorForm::optionOpen_Click);
			// 
			// toolStripSeparator1
			// 
			this->toolStripSeparator1->Name = L"toolStripSeparator1";
			this->toolStripSeparator1->Size = System::Drawing::Size(282, 6);
			// 
			// optionSave
			// 
			this->optionSave->Enabled = false;
			this->optionSave->Name = L"optionSave";
			this->optionSave->ShortcutKeys = static_cast<System::Windows::Forms::Keys>((System::Windows::Forms::Keys::Control | System::Windows::Forms::Keys::S));
			this->optionSave->Size = System::Drawing::Size(285, 34);
			this->optionSave->Text = L"Save";
			this->optionSave->ToolTipText = L"Save file(s)";
			this->optionSave->Click += gcnew System::EventHandler(this, &HumongousEditorForm::optionSave_Click);
			// 
			// optionSaveAs
			// 
			this->optionSaveAs->Enabled = false;
			this->optionSaveAs->Name = L"optionSaveAs";
			this->optionSaveAs->ShortcutKeys = static_cast<System::Windows::Forms::Keys>(((System::Windows::Forms::Keys::Control | System::Windows::Forms::Keys::Shift)
				| System::Windows::Forms::Keys::S));
			this->optionSaveAs->Size = System::Drawing::Size(285, 34);
			this->optionSaveAs->Text = L"Save As";
			this->optionSaveAs->ToolTipText = L"Save file(s) as";
			this->optionSaveAs->Click += gcnew System::EventHandler(this, &HumongousEditorForm::optionSaveAs_Click);
			// 
			// toolStripSeparator2
			// 
			this->toolStripSeparator2->Name = L"toolStripSeparator2";
			this->toolStripSeparator2->Size = System::Drawing::Size(282, 6);
			// 
			// optionIndex
			// 
			this->optionIndex->Name = L"optionIndex";
			this->optionIndex->ShortcutKeys = static_cast<System::Windows::Forms::Keys>((System::Windows::Forms::Keys::Control | System::Windows::Forms::Keys::I));
			this->optionIndex->Size = System::Drawing::Size(285, 34);
			this->optionIndex->Text = L"Index";
			this->optionIndex->ToolTipText = L"Create index of file";
			this->optionIndex->Click += gcnew System::EventHandler(this, &HumongousEditorForm::optionIndex_Click);
			// 
			// optionDecrypt
			// 
			this->optionDecrypt->Name = L"optionDecrypt";
			this->optionDecrypt->ShortcutKeys = static_cast<System::Windows::Forms::Keys>((System::Windows::Forms::Keys::Control | System::Windows::Forms::Keys::D));
			this->optionDecrypt->Size = System::Drawing::Size(285, 34);
			this->optionDecrypt->Text = L"Decrypt";
			this->optionDecrypt->ToolTipText = L"Encrypt/Decrypt file";
			this->optionDecrypt->Click += gcnew System::EventHandler(this, &HumongousEditorForm::optionDecrypt_Click);
			// 
			// toolStripSeparator3
			// 
			this->toolStripSeparator3->Name = L"toolStripSeparator3";
			this->toolStripSeparator3->Size = System::Drawing::Size(282, 6);
			// 
			// optionQuit
			// 
			this->optionQuit->Name = L"optionQuit";
			this->optionQuit->ShortcutKeys = static_cast<System::Windows::Forms::Keys>((System::Windows::Forms::Keys::Control | System::Windows::Forms::Keys::Q));
			this->optionQuit->Size = System::Drawing::Size(285, 34);
			this->optionQuit->Text = L"Exit";
			this->optionQuit->ToolTipText = L"Quit";
			// 
			// helpMenu
			// 
			this->helpMenu->DropDownItems->AddRange(gcnew cli::array< System::Windows::Forms::ToolStripItem^  >(1) { this->optionAbout });
			this->helpMenu->Name = L"helpMenu";
			this->helpMenu->Size = System::Drawing::Size(65, 29);
			this->helpMenu->Text = L"Help";
			// 
			// optionAbout
			// 
			this->optionAbout->Name = L"optionAbout";
			this->optionAbout->ShortcutKeys = System::Windows::Forms::Keys::F1;
			this->optionAbout->Size = System::Drawing::Size(195, 34);
			this->optionAbout->Text = L"About";
			this->optionAbout->ToolTipText = L"About this program";
			this->optionAbout->Click += gcnew System::EventHandler(this, &HumongousEditorForm::optionAbout_Click);
			// 
			// topMenu
			// 
			this->topMenu->BackColor = System::Drawing::SystemColors::Window;
			this->topMenu->GripMargin = System::Windows::Forms::Padding(2, 2, 0, 2);
			this->topMenu->ImageScalingSize = System::Drawing::Size(22, 22);
			this->topMenu->Items->AddRange(gcnew cli::array< System::Windows::Forms::ToolStripItem^  >(2) { this->optionSeperator1, this->helpMenu });
			this->topMenu->Location = System::Drawing::Point(0, 0);
			this->topMenu->Name = L"topMenu";
			this->topMenu->Size = System::Drawing::Size(991, 33);
			this->topMenu->TabIndex = 0;
			this->topMenu->Text = L"menuStrip2";
			// 
			// openButton
			// 
			this->openButton->BackColor = System::Drawing::Color::Transparent;
			this->openButton->BackgroundImage = (cli::safe_cast<System::Drawing::Image^>(resources->GetObject(L"openButton.BackgroundImage")));
			this->openButton->BackgroundImageLayout = System::Windows::Forms::ImageLayout::Stretch;
			this->openButton->FlatAppearance->BorderSize = 0;
			this->openButton->FlatStyle = System::Windows::Forms::FlatStyle::Flat;
			this->openButton->Location = System::Drawing::Point(7, 2);
			this->openButton->Name = L"openButton";
			this->openButton->Size = System::Drawing::Size(40, 40);
			this->openButton->TabIndex = 4;
			this->openButton->UseVisualStyleBackColor = false;
			this->openButton->Click += gcnew System::EventHandler(this, &HumongousEditorForm::optionOpen_Click);
			this->openButton->MouseEnter += gcnew System::EventHandler(this, &HumongousEditorForm::buttonHover);
			this->openButton->MouseLeave += gcnew System::EventHandler(this, &HumongousEditorForm::buttonExit);
			// 
			// saveButton
			// 
			this->saveButton->BackColor = System::Drawing::Color::Transparent;
			this->saveButton->BackgroundImage = (cli::safe_cast<System::Drawing::Image^>(resources->GetObject(L"saveButton.BackgroundImage")));
			this->saveButton->BackgroundImageLayout = System::Windows::Forms::ImageLayout::Stretch;
			this->saveButton->Enabled = false;
			this->saveButton->FlatAppearance->BorderSize = 0;
			this->saveButton->FlatStyle = System::Windows::Forms::FlatStyle::Flat;
			this->saveButton->Location = System::Drawing::Point(62, 2);
			this->saveButton->Name = L"saveButton";
			this->saveButton->Size = System::Drawing::Size(40, 40);
			this->saveButton->TabIndex = 5;
			this->saveButton->UseVisualStyleBackColor = false;
			this->saveButton->Click += gcnew System::EventHandler(this, &HumongousEditorForm::optionSave_Click);
			this->saveButton->MouseEnter += gcnew System::EventHandler(this, &HumongousEditorForm::buttonHover);
			this->saveButton->MouseLeave += gcnew System::EventHandler(this, &HumongousEditorForm::buttonExit);
			// 
			// aboutButton
			// 
			this->aboutButton->BackColor = System::Drawing::Color::Transparent;
			this->aboutButton->BackgroundImage = (cli::safe_cast<System::Drawing::Image^>(resources->GetObject(L"aboutButton.BackgroundImage")));
			this->aboutButton->BackgroundImageLayout = System::Windows::Forms::ImageLayout::Stretch;
			this->aboutButton->FlatAppearance->BorderSize = 0;
			this->aboutButton->FlatStyle = System::Windows::Forms::FlatStyle::Flat;
			this->aboutButton->ForeColor = System::Drawing::Color::Transparent;
			this->aboutButton->Location = System::Drawing::Point(266, 3);
			this->aboutButton->Name = L"aboutButton";
			this->aboutButton->Size = System::Drawing::Size(40, 40);
			this->aboutButton->TabIndex = 6;
			this->aboutButton->UseVisualStyleBackColor = false;
			this->aboutButton->Click += gcnew System::EventHandler(this, &HumongousEditorForm::optionAbout_Click);
			this->aboutButton->MouseEnter += gcnew System::EventHandler(this, &HumongousEditorForm::buttonHover);
			this->aboutButton->MouseLeave += gcnew System::EventHandler(this, &HumongousEditorForm::buttonExit);
			// 
			// panel2
			// 
			this->panel2->BackColor = System::Drawing::SystemColors::ButtonShadow;
			this->panel2->Location = System::Drawing::Point(54, 4);
			this->panel2->Name = L"panel2";
			this->panel2->Size = System::Drawing::Size(1, 36);
			this->panel2->TabIndex = 7;
			// 
			// saveAsButton
			// 
			this->saveAsButton->BackColor = System::Drawing::Color::Transparent;
			this->saveAsButton->BackgroundImage = (cli::safe_cast<System::Drawing::Image^>(resources->GetObject(L"saveAsButton.BackgroundImage")));
			this->saveAsButton->BackgroundImageLayout = System::Windows::Forms::ImageLayout::Stretch;
			this->saveAsButton->Enabled = false;
			this->saveAsButton->FlatAppearance->BorderSize = 0;
			this->saveAsButton->FlatStyle = System::Windows::Forms::FlatStyle::Flat;
			this->saveAsButton->Location = System::Drawing::Point(109, 2);
			this->saveAsButton->Name = L"saveAsButton";
			this->saveAsButton->Size = System::Drawing::Size(40, 40);
			this->saveAsButton->TabIndex = 8;
			this->saveAsButton->UseVisualStyleBackColor = false;
			this->saveAsButton->Click += gcnew System::EventHandler(this, &HumongousEditorForm::optionSaveAs_Click);
			this->saveAsButton->MouseEnter += gcnew System::EventHandler(this, &HumongousEditorForm::buttonHover);
			this->saveAsButton->MouseLeave += gcnew System::EventHandler(this, &HumongousEditorForm::buttonExit);
			// 
			// panel3
			// 
			this->panel3->BackColor = System::Drawing::SystemColors::ButtonShadow;
			this->panel3->Location = System::Drawing::Point(156, 4);
			this->panel3->Name = L"panel3";
			this->panel3->Size = System::Drawing::Size(1, 36);
			this->panel3->TabIndex = 9;
			// 
			// panel4
			// 
			this->panel4->BackColor = System::Drawing::SystemColors::ButtonShadow;
			this->panel4->Location = System::Drawing::Point(258, 4);
			this->panel4->Name = L"panel4";
			this->panel4->Size = System::Drawing::Size(1, 36);
			this->panel4->TabIndex = 9;
			// 
			// decryptButton
			// 
			this->decryptButton->BackColor = System::Drawing::Color::Transparent;
			this->decryptButton->BackgroundImage = (cli::safe_cast<System::Drawing::Image^>(resources->GetObject(L"decryptButton.BackgroundImage")));
			this->decryptButton->BackgroundImageLayout = System::Windows::Forms::ImageLayout::Stretch;
			this->decryptButton->FlatAppearance->BorderSize = 0;
			this->decryptButton->FlatStyle = System::Windows::Forms::FlatStyle::Flat;
			this->decryptButton->ForeColor = System::Drawing::Color::Transparent;
			this->decryptButton->Location = System::Drawing::Point(211, 2);
			this->decryptButton->Name = L"decryptButton";
			this->decryptButton->Size = System::Drawing::Size(40, 40);
			this->decryptButton->TabIndex = 10;
			this->decryptButton->TabStop = false;
			this->decryptButton->UseVisualStyleBackColor = false;
			this->decryptButton->Click += gcnew System::EventHandler(this, &HumongousEditorForm::optionDecrypt_Click);
			this->decryptButton->MouseEnter += gcnew System::EventHandler(this, &HumongousEditorForm::buttonHover);
			this->decryptButton->MouseLeave += gcnew System::EventHandler(this, &HumongousEditorForm::buttonExit);
			// 
			// actionPanel
			// 
			this->actionPanel->BackColor = System::Drawing::SystemColors::GradientInactiveCaption;
			this->actionPanel->Controls->Add(this->indexButton);
			this->actionPanel->Controls->Add(this->decryptButton);
			this->actionPanel->Controls->Add(this->panel3);
			this->actionPanel->Controls->Add(this->panel4);
			this->actionPanel->Controls->Add(this->saveAsButton);
			this->actionPanel->Controls->Add(this->panel2);
			this->actionPanel->Controls->Add(this->aboutButton);
			this->actionPanel->Controls->Add(this->saveButton);
			this->actionPanel->Controls->Add(this->openButton);
			this->actionPanel->Dock = System::Windows::Forms::DockStyle::Top;
			this->actionPanel->Location = System::Drawing::Point(0, 33);
			this->actionPanel->Name = L"actionPanel";
			this->actionPanel->Padding = System::Windows::Forms::Padding(10);
			this->actionPanel->Size = System::Drawing::Size(991, 44);
			this->actionPanel->TabIndex = 4;
			// 
			// indexButton
			// 
			this->indexButton->BackColor = System::Drawing::Color::Transparent;
			this->indexButton->BackgroundImage = (cli::safe_cast<System::Drawing::Image^>(resources->GetObject(L"indexButton.BackgroundImage")));
			this->indexButton->BackgroundImageLayout = System::Windows::Forms::ImageLayout::Stretch;
			this->indexButton->FlatAppearance->BorderSize = 0;
			this->indexButton->FlatStyle = System::Windows::Forms::FlatStyle::Flat;
			this->indexButton->ForeColor = System::Drawing::Color::Transparent;
			this->indexButton->Location = System::Drawing::Point(164, 2);
			this->indexButton->Name = L"indexButton";
			this->indexButton->Size = System::Drawing::Size(40, 40);
			this->indexButton->TabIndex = 11;
			this->indexButton->TabStop = false;
			this->indexButton->UseVisualStyleBackColor = false;
			this->indexButton->Click += gcnew System::EventHandler(this, &HumongousEditorForm::optionIndex_Click);
			this->indexButton->MouseEnter += gcnew System::EventHandler(this, &HumongousEditorForm::buttonHover);
			this->indexButton->MouseLeave += gcnew System::EventHandler(this, &HumongousEditorForm::buttonExit);
			// 
			// tabWelcome
			// 
			this->tabWelcome->BorderStyle = System::Windows::Forms::BorderStyle::FixedSingle;
			this->tabWelcome->Location = System::Drawing::Point(4, 29);
			this->tabWelcome->Name = L"tabWelcome";
			this->tabWelcome->Size = System::Drawing::Size(398, 413);
			this->tabWelcome->TabIndex = 0;
			this->tabWelcome->Text = L"Welcome!";
			this->tabWelcome->UseVisualStyleBackColor = true;
			// 
			// HumongousEditorForm
			// 
			this->ClientSize = System::Drawing::Size(991, 544);
			this->Controls->Add(this->actionPanel);
			this->Controls->Add(this->splitContainer);
			this->Controls->Add(this->topMenu);
			this->MainMenuStrip = this->topMenu;
			this->Name = L"HumongousEditorForm";
			this->Load += gcnew System::EventHandler(this, &HumongousEditorForm::Form1_Load);
			this->splitContainer->Panel1->ResumeLayout(false);
			this->splitContainer->Panel2->ResumeLayout(false);
			(cli::safe_cast<System::ComponentModel::ISupportInitialize^>(this->splitContainer))->EndInit();
			this->splitContainer->ResumeLayout(false);
			this->tabControl1->ResumeLayout(false);
			this->topMenu->ResumeLayout(false);
			this->topMenu->PerformLayout();
			this->actionPanel->ResumeLayout(false);
			this->ResumeLayout(false);
			this->PerformLayout();

		}
#pragma endregion
		System::Void Form1_Load(System::Object^ sender, System::EventArgs^ e)
		{
			tooltipAbout->SetToolTip(this->aboutButton, "About this program");
			tooltipOpen->SetToolTip(this->openButton, "Open file");
			tooltipSave->SetToolTip(this->saveButton, "Save file(s)");
			tooltipSaveAs->SetToolTip(this->saveAsButton, "Save file(s) as");
			tooltipDecrypt->SetToolTip(this->decryptButton, "Encrypt/Decrypt file");
			tooltipIndex->SetToolTip(this->indexButton, "Create index of file");
		}
		// Opens a window with info about the application (via the top menu or info button).
		System::Void optionAbout_Click(System::Object^ sender, System::EventArgs^ e)
		{
			MessageBox::Show("Humongous File Editor by Kylian Dekker\nVersion: 0.0.1\n\nMany thanks to rzil for helping me with reading the resource files.", "About", MessageBoxButtons::OK, MessageBoxIcon::Information);
		}
		// Hover animations for buttons.
		System::Void buttonHover(System::Object^ sender, System::EventArgs^ e)
		{
			Button^ button = static_cast<Button^>(sender);

			button->BackColor = System::Drawing::SystemColors::GradientActiveCaption;
			button->FlatStyle = System::Windows::Forms::FlatStyle::Flat;
		}
		// Hover animations for buttons.
		System::Void buttonExit(System::Object^ sender, System::EventArgs^ e)
		{
			Button^ button = static_cast<Button^>(sender);
			button->BackColor = System::Drawing::Color::Transparent;
			button->FlatStyle = System::Windows::Forms::FlatStyle::Flat;
		}
		/// <summary>
		/// Returns all the filters for FILE dialogs.
		/// </summary>
		/// <returns></returns>
		LPCWSTR getFilter()
		{
			return L"\
						Humongous Game Files (*.HE0, *.HE2, *.HE3, *.HE4)\
						\0*.HE0;*.HE2;*.HE3;*.HE4\0\
						Humongous Talkie Files (*.HE2)\
						\0*.HE2\0\
						Humongous Index Files (*.HE0)\
						\0*.HE0\0\
						Humongous Song Files (*.HE4)\
						\0*.HE4\0";
		}
		// Opens a window with info about the application (via the top menu or info button).
		System::Void optionIndex_Click(System::Object^ sender, System::EventArgs^ e)
		{
			chunk_reader::FileIndexer f = chunk_reader::FileIndexer();
			if (f.Read())
			{
				System::Windows::Forms::MessageBox::Show("Successfully indexed file.", "Success", System::Windows::Forms::MessageBoxButtons::OK, System::Windows::Forms::MessageBoxIcon::Information);
				return;
			}
		}
		System::Void optionOpen_Click(System::Object^ sender, System::EventArgs^ e)
		{
			OPENFILENAME ofn;
			TCHAR sz_file[260] = { 0 };

			ZeroMemory(&ofn, sizeof(ofn));
			ofn.lStructSize = sizeof(ofn);
			ofn.lpstrFile = sz_file;
			ofn.nMaxFile = sizeof(sz_file);

			ofn.lpstrFilter = getFilter();
			ofn.nFilterIndex = 1;
			ofn.lpstrFileTitle = nullptr;
			ofn.nMaxFileTitle = 0;
			ofn.lpstrInitialDir = nullptr;
			ofn.Flags = OFN_PATHMUSTEXIST | OFN_FILEMUSTEXIST;

			if (GetOpenFileNameW(&ofn))
			{
				const auto path = new char[wcslen(ofn.lpstrFile) + 1];
				wsprintfA(path, "%S", ofn.lpstrFile);

				chunk_reader::ResourceGatherer f = chunk_reader::ResourceGatherer();
				if (f.Read(path))
				{
					System::Windows::Forms::MessageBox::Show("Successfully opened file.", "Success", System::Windows::Forms::MessageBoxButtons::OK, System::Windows::Forms::MessageBoxIcon::Information);

					this->optionSave->Enabled = true;
					this->optionSaveAs->Enabled = true;
					this->saveButton->Enabled = true;
					this->saveAsButton->Enabled = true;
				}

				delete[] path;
			}
			HideProgressBar();
		}
		System::Void optionSave_Click(System::Object^ sender, System::EventArgs^ e)
		{
			System::Windows::Forms::DialogResult result = System::Windows::Forms::MessageBox::Show("Are you sure you want to overwrite the current opened files?", "Overwrite current files", System::Windows::Forms::MessageBoxButtons::YesNo, System::Windows::Forms::MessageBoxIcon::Information);
			
			if (result == System::Windows::Forms::DialogResult::Yes)
				if (files::FILES.Save())
					System::Windows::Forms::MessageBox::Show("Successfully saved files.", "Success", System::Windows::Forms::MessageBoxButtons::OK, System::Windows::Forms::MessageBoxIcon::Information);
			HideProgressBar();
		}
		System::Void optionSaveAs_Click(System::Object^ sender, System::EventArgs^ e)
		{
			if (files::FILES.SaveAs())
				System::Windows::Forms::MessageBox::Show("Successfully saved files.", "Success", System::Windows::Forms::MessageBoxButtons::OK, System::Windows::Forms::MessageBoxIcon::Information);
			HideProgressBar();
		}
		// Double click on a node.
		System::Void entryView_DoubleClick(System::Object^ sender, System::Windows::Forms::MouseEventArgs^ e)
		{
			if (e->Button != System::Windows::Forms::MouseButtons::Left)
				return;

			System::Windows::Forms::TreeView^ view = static_cast<System::Windows::Forms::TreeView^>(sender);
			HumongousNode^ node = static_cast<HumongousNode^>(view->SelectedNode);

			if (!view->SelectedNode)
				return;

			if (!tabControl1->Controls->ContainsKey(node->Name))
				AddTab(node);
			tabControl1->SelectedIndex = tabControl1->Controls->IndexOfKey(node->Name);
		}
	public:
		System::Void ShowProgressBar()
		{
			progressBar.Location = Point(this->Location.X + (this->Size.Width / 2), this->Location.Y + (this->Size.Height / 2));
			progressBar.Show();
			progressBar.Focus();
			progressBar.Activate();
		}
		System::Void HideProgressBar()
		{
			progressBar.Hide();
		}
		/// <summary>
		/// Adds a new tab to the tab control.
		/// </summary>
		/// <param name="node">The original node that was clicked on.</param>
		/// <param name="entry">The entry.</param>
		void AddTab(HumongousNode^ node)
		{
			TabFunctions^ tf = gcnew TabFunctions();
			tf->AddTab(node, this->tabControl1);
		}
		System::Windows::Forms::TreeNode^ GetBaseNode(System::String^ name)
		{
			for (size_t i = 0; i < this->entryView->Nodes->Count; i++)
			{
				if (this->entryView->Nodes[i]->Name == name)
				{
					this->entryView->Nodes[i]->Nodes->Clear();
					return this->entryView->Nodes[i];
				}
			}

			System::Windows::Forms::TreeNode^ baseNode;
			baseNode = (gcnew System::Windows::Forms::TreeNode);
			baseNode->Name = name;
			baseNode->Text = baseNode->Name;
			this->entryView->Nodes->Add(baseNode);
			return baseNode;
		}
		System::Windows::Forms::TreeNode^ GetNode(System::Windows::Forms::TreeNode^ baseNode, System::String^ name)
		{
			for (size_t i = 0; i < baseNode->Nodes->Count; i++)
				if (baseNode->Nodes[i]->Name == name)
					return baseNode->Nodes[i];

			System::Windows::Forms::TreeNode^ node;
			node = (gcnew System::Windows::Forms::TreeNode);
			node->Name = name;
			node->Text = node->Name;
			baseNode->Nodes->Add(node);
			return node;
		}
		System::Void tabControl1_MouseDown(System::Object^ sender, System::Windows::Forms::MouseEventArgs^ e)
		{
			if (e->Button == System::Windows::Forms::MouseButtons::Left)
			{
				System::Drawing::Rectangle^ r = this->tabControl1->GetTabRect(this->tabControl1->SelectedIndex);
				r->Offset(2, 2);
				r->Width = 5;
				r->Height = 5;
				if (!r->Contains(e->Location))
					return;
			}
			if (e->Button == System::Windows::Forms::MouseButtons::Middle)
			{
				System::Drawing::Rectangle^ r = this->tabControl1->GetTabRect(this->tabControl1->SelectedIndex);
				if (!r->Contains(e->Location))
					return;
			}
			this->tabControl1->TabPages->Remove(this->tabControl1->SelectedTab);
		}
		System::Void tabControl1_DrawX(System::Object^ sender, System::Windows::Forms::DrawItemEventArgs^ e)
		{
			System::Drawing::Rectangle^ r = e->Bounds;
			r = this->tabControl1->GetTabRect(e->Index);
			r->Offset(2, 2);
			r->Width = 5;
			r->Height = 5;
			System::Drawing::Brush^ b = (gcnew System::Drawing::SolidBrush(System::Drawing::Color::Black));
			System::Drawing::Pen^ p = (gcnew System::Drawing::Pen(b));
			e->Graphics->DrawLine(p, r->X, r->Y, r->X + r->Width, r->Y + r->Height);
			e->Graphics->DrawLine(p, r->X + r->Width, r->Y, r->X, r->Y + r->Height);

			System::String^ titel = this->tabControl1->TabPages[e->Index]->Text;
			System::Drawing::Font^ f = this->Font;
			System::Drawing::PointF pointF = System::Drawing::PointF(static_cast<float>(r->X + 5), static_cast<float>(r->Y));
			e->Graphics->DrawString(titel, f, b, pointF);
		}
		System::Void optionDecrypt_Click(System::Object^ sender, System::EventArgs^ e)
		{
			chunk_reader::FileDecrypter fileDecrypter;
			if (fileDecrypter.Read())
			{
				System::Windows::Forms::MessageBox::Show("Successfully decrypted/encrypted file.", "Success", System::Windows::Forms::MessageBoxButtons::OK, System::Windows::Forms::MessageBoxIcon::Information);
				return;
			}
		}
	};
}