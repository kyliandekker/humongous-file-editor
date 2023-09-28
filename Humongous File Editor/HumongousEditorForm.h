#pragma once

#include <string>
#include <windows.h>

#include "functions/FileIndexer.h"
#include "functions/FileDecrypter.h"
#include "functions/ResourceGatherer.h"
#include "forms/HumongousNode.h"
#include "forms/HumongousButton.h"
#include "functions/TabFunctions.h"

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
		System::Windows::Forms::ToolStripMenuItem^ optionQuit;

		System::Windows::Forms::ToolStripMenuItem^ helpMenu;
		System::Windows::Forms::ToolStripMenuItem^ optionAbout;

		System::Windows::Forms::MenuStrip^ topMenu;

		System::Windows::Forms::StatusStrip^ statusStrip2;
		System::Windows::Forms::ToolStripProgressBar^ toolProgressBar;
		System::Windows::Forms::TreeView^ entryView;
		System::Windows::Forms::Panel^ treeActionPanel;

		System::Windows::Forms::Button^ exportSelected;
		System::Windows::Forms::Button^ unselectAll;
		System::Windows::Forms::Button^ selectAllButton;

		System::Windows::Forms::ToolTip^ tooltipAbout = gcnew System::Windows::Forms::ToolTip();
		System::Windows::Forms::ToolTip^ tooltipEncrypt = gcnew System::Windows::Forms::ToolTip();
		System::Windows::Forms::ToolTip^ tooltipIndex = gcnew System::Windows::Forms::ToolTip();

		System::Windows::Forms::ToolStripMenuItem^ optionSave;
		System::Windows::Forms::ToolStripSeparator^ toolStripSeparator1;


		System::String^ windowTitle = gcnew System::String("Humongous File Editor");
    public: System::Windows::Forms::Button^ openButton;
    public: System::Windows::Forms::Button^ saveButton;
    public: System::Windows::Forms::Button^ aboutButton;
    public: System::Windows::Forms::Panel^ panel2;
    public: System::Windows::Forms::Button^ saveAsButton;
    public: System::Windows::Forms::Panel^ panel3;
    public: System::Windows::Forms::Panel^ panel4;
    public: System::Windows::Forms::Button^ encryptButton;
	public: System::Windows::Forms::Panel^ actionPanel;
	public: System::Windows::Forms::Button^ indexButton;
	public: System::Windows::Forms::TabPage^ tabWelcome;
		  System::Windows::Forms::TabControl^ tabControl1;
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
			this->treeActionPanel = (gcnew System::Windows::Forms::Panel());
			this->exportSelected = (gcnew System::Windows::Forms::Button());
			this->unselectAll = (gcnew System::Windows::Forms::Button());
			this->selectAllButton = (gcnew System::Windows::Forms::Button());
			this->entryView = (gcnew System::Windows::Forms::TreeView());
			this->tabControl1 = (gcnew System::Windows::Forms::TabControl());
			this->tabWelcome = (gcnew System::Windows::Forms::TabPage());
			this->optionSeperator1 = (gcnew System::Windows::Forms::ToolStripMenuItem());
			this->optionOpen = (gcnew System::Windows::Forms::ToolStripMenuItem());
			this->toolStripSeparator1 = (gcnew System::Windows::Forms::ToolStripSeparator());
			this->optionSave = (gcnew System::Windows::Forms::ToolStripMenuItem());
			this->optionSaveAs = (gcnew System::Windows::Forms::ToolStripMenuItem());
			this->toolStripSeparator2 = (gcnew System::Windows::Forms::ToolStripSeparator());
			this->optionQuit = (gcnew System::Windows::Forms::ToolStripMenuItem());
			this->helpMenu = (gcnew System::Windows::Forms::ToolStripMenuItem());
			this->optionAbout = (gcnew System::Windows::Forms::ToolStripMenuItem());
			this->topMenu = (gcnew System::Windows::Forms::MenuStrip());
			this->statusStrip2 = (gcnew System::Windows::Forms::StatusStrip());
			this->toolProgressBar = (gcnew System::Windows::Forms::ToolStripProgressBar());
			this->openButton = (gcnew System::Windows::Forms::Button());
			this->saveButton = (gcnew System::Windows::Forms::Button());
			this->aboutButton = (gcnew System::Windows::Forms::Button());
			this->panel2 = (gcnew System::Windows::Forms::Panel());
			this->saveAsButton = (gcnew System::Windows::Forms::Button());
			this->panel3 = (gcnew System::Windows::Forms::Panel());
			this->panel4 = (gcnew System::Windows::Forms::Panel());
			this->encryptButton = (gcnew System::Windows::Forms::Button());
			this->actionPanel = (gcnew System::Windows::Forms::Panel());
			this->indexButton = (gcnew System::Windows::Forms::Button());
			(cli::safe_cast<System::ComponentModel::ISupportInitialize^>(this->splitContainer))->BeginInit();
			this->splitContainer->Panel1->SuspendLayout();
			this->splitContainer->Panel2->SuspendLayout();
			this->splitContainer->SuspendLayout();
			this->treeActionPanel->SuspendLayout();
			this->tabControl1->SuspendLayout();
			this->topMenu->SuspendLayout();
			this->statusStrip2->SuspendLayout();
			this->actionPanel->SuspendLayout();
			this->SuspendLayout();
			// 
			// splitContainer
			// 
			this->splitContainer->Anchor = static_cast<System::Windows::Forms::AnchorStyles>((((System::Windows::Forms::AnchorStyles::Top | System::Windows::Forms::AnchorStyles::Bottom)
				| System::Windows::Forms::AnchorStyles::Left)
				| System::Windows::Forms::AnchorStyles::Right));
			this->splitContainer->BackColor = System::Drawing::SystemColors::ControlLight;
			this->splitContainer->Location = System::Drawing::Point(0, 64);
			this->splitContainer->Name = L"splitContainer";
			// 
			// splitContainer.Panel1
			// 
			this->splitContainer->Panel1->BackColor = System::Drawing::SystemColors::Window;
			this->splitContainer->Panel1->Controls->Add(this->treeActionPanel);
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
			// treeActionPanel
			// 
			this->treeActionPanel->Controls->Add(this->exportSelected);
			this->treeActionPanel->Controls->Add(this->unselectAll);
			this->treeActionPanel->Controls->Add(this->selectAllButton);
			this->treeActionPanel->Dock = System::Windows::Forms::DockStyle::Top;
			this->treeActionPanel->Location = System::Drawing::Point(10, 10);
			this->treeActionPanel->Name = L"treeActionPanel";
			this->treeActionPanel->Size = System::Drawing::Size(541, 29);
			this->treeActionPanel->TabIndex = 1;
			// 
			// exportSelected
			// 
			this->exportSelected->Enabled = false;
			this->exportSelected->Location = System::Drawing::Point(165, 3);
			this->exportSelected->Name = L"exportSelected";
			this->exportSelected->Size = System::Drawing::Size(102, 23);
			this->exportSelected->TabIndex = 2;
			this->exportSelected->Text = L"Export Selected";
			this->exportSelected->UseVisualStyleBackColor = true;
			// 
			// unselectAll
			// 
			this->unselectAll->Location = System::Drawing::Point(84, 3);
			this->unselectAll->Name = L"unselectAll";
			this->unselectAll->Size = System::Drawing::Size(75, 23);
			this->unselectAll->TabIndex = 1;
			this->unselectAll->Text = L"Select None";
			this->unselectAll->UseVisualStyleBackColor = true;
			// 
			// selectAllButton
			// 
			this->selectAllButton->Location = System::Drawing::Point(3, 3);
			this->selectAllButton->Name = L"selectAllButton";
			this->selectAllButton->Size = System::Drawing::Size(75, 23);
			this->selectAllButton->TabIndex = 0;
			this->selectAllButton->Text = L"Select All";
			this->selectAllButton->UseVisualStyleBackColor = true;
			this->selectAllButton->Click += gcnew System::EventHandler(this, &HumongousEditorForm::selectAllButton_Click);
			// 
			// entryView
			// 
			this->entryView->Anchor = static_cast<System::Windows::Forms::AnchorStyles>((((System::Windows::Forms::AnchorStyles::Top | System::Windows::Forms::AnchorStyles::Bottom)
				| System::Windows::Forms::AnchorStyles::Left)
				| System::Windows::Forms::AnchorStyles::Right));
			this->entryView->BackColor = System::Drawing::Color::FromArgb(static_cast<System::Int32>(static_cast<System::Byte>(249)), static_cast<System::Int32>(static_cast<System::Byte>(249)),
				static_cast<System::Int32>(static_cast<System::Byte>(249)));
			this->entryView->ItemHeight = 20;
			this->entryView->Location = System::Drawing::Point(10, 45);
			this->entryView->Name = L"entryView";
			this->entryView->Size = System::Drawing::Size(541, 409);
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
			// tabWelcome
			// 
			this->tabWelcome->Location = System::Drawing::Point(4, 22);
			this->tabWelcome->Name = L"tabWelcome";
			this->tabWelcome->Size = System::Drawing::Size(398, 420);
			this->tabWelcome->TabIndex = 0;
			this->tabWelcome->Text = L"Welcome!";
			this->tabWelcome->UseVisualStyleBackColor = true;
			// 
			// optionSeperator1
			// 
			this->optionSeperator1->DropDownItems->AddRange(gcnew cli::array< System::Windows::Forms::ToolStripItem^  >(6) {
				this->optionOpen,
					this->toolStripSeparator1, this->optionSave, this->optionSaveAs, this->toolStripSeparator2, this->optionQuit
			});
			this->optionSeperator1->Name = L"optionSeperator1";
			this->optionSeperator1->Size = System::Drawing::Size(37, 20);
			this->optionSeperator1->Text = L"File";
			// 
			// optionOpen
			// 
			this->optionOpen->Name = L"optionOpen";
			this->optionOpen->ShortcutKeys = static_cast<System::Windows::Forms::Keys>((System::Windows::Forms::Keys::Control | System::Windows::Forms::Keys::O));
			this->optionOpen->Size = System::Drawing::Size(186, 22);
			this->optionOpen->Text = L"Open";
			this->optionOpen->Click += gcnew System::EventHandler(this, &HumongousEditorForm::optionOpen_Click);
			// 
			// toolStripSeparator1
			// 
			this->toolStripSeparator1->Name = L"toolStripSeparator1";
			this->toolStripSeparator1->Size = System::Drawing::Size(183, 6);
			// 
			// optionSave
			// 
			this->optionSave->Enabled = false;
			this->optionSave->Name = L"optionSave";
			this->optionSave->ShortcutKeys = static_cast<System::Windows::Forms::Keys>((System::Windows::Forms::Keys::Control | System::Windows::Forms::Keys::S));
			this->optionSave->Size = System::Drawing::Size(186, 22);
			this->optionSave->Text = L"Save";
			// 
			// optionSaveAs
			// 
			this->optionSaveAs->Enabled = false;
			this->optionSaveAs->Name = L"optionSaveAs";
			this->optionSaveAs->ShortcutKeys = static_cast<System::Windows::Forms::Keys>(((System::Windows::Forms::Keys::Control | System::Windows::Forms::Keys::Shift)
				| System::Windows::Forms::Keys::S));
			this->optionSaveAs->Size = System::Drawing::Size(186, 22);
			this->optionSaveAs->Text = L"Save As";
			// 
			// toolStripSeparator2
			// 
			this->toolStripSeparator2->Name = L"toolStripSeparator2";
			this->toolStripSeparator2->Size = System::Drawing::Size(183, 6);
			// 
			// optionQuit
			// 
			this->optionQuit->Name = L"optionQuit";
			this->optionQuit->ShortcutKeys = static_cast<System::Windows::Forms::Keys>((System::Windows::Forms::Keys::Control | System::Windows::Forms::Keys::Q));
			this->optionQuit->Size = System::Drawing::Size(186, 22);
			this->optionQuit->Text = L"Exit";
			// 
			// helpMenu
			// 
			this->helpMenu->DropDownItems->AddRange(gcnew cli::array< System::Windows::Forms::ToolStripItem^  >(1) { this->optionAbout });
			this->helpMenu->Name = L"helpMenu";
			this->helpMenu->Size = System::Drawing::Size(44, 20);
			this->helpMenu->Text = L"Help";
			// 
			// optionAbout
			// 
			this->optionAbout->Name = L"optionAbout";
			this->optionAbout->Size = System::Drawing::Size(107, 22);
			this->optionAbout->Text = L"About";
			this->optionAbout->Click += gcnew System::EventHandler(this, &HumongousEditorForm::optionAbout_Click);
			// 
			// topMenu
			// 
			this->topMenu->BackColor = System::Drawing::SystemColors::Window;
			this->topMenu->ImageScalingSize = System::Drawing::Size(22, 22);
			this->topMenu->Items->AddRange(gcnew cli::array< System::Windows::Forms::ToolStripItem^  >(2) { this->optionSeperator1, this->helpMenu });
			this->topMenu->Location = System::Drawing::Point(0, 0);
			this->topMenu->Name = L"topMenu";
			this->topMenu->Size = System::Drawing::Size(991, 24);
			this->topMenu->TabIndex = 0;
			this->topMenu->Text = L"menuStrip2";
			// 
			// statusStrip2
			// 
			this->statusStrip2->ImageScalingSize = System::Drawing::Size(22, 22);
			this->statusStrip2->Items->AddRange(gcnew cli::array< System::Windows::Forms::ToolStripItem^  >(1) { this->toolProgressBar });
			this->statusStrip2->Location = System::Drawing::Point(0, 536);
			this->statusStrip2->Margin = System::Windows::Forms::Padding(5);
			this->statusStrip2->Name = L"statusStrip2";
			this->statusStrip2->Size = System::Drawing::Size(991, 25);
			this->statusStrip2->TabIndex = 2;
			this->statusStrip2->Text = L"statusStrip2";
			// 
			// toolProgressBar
			// 
			this->toolProgressBar->Name = L"toolProgressBar";
			this->toolProgressBar->Size = System::Drawing::Size(100, 19);
			// 
			// openButton
			// 
			this->openButton->BackColor = System::Drawing::Color::Transparent;
			this->openButton->BackgroundImage = (cli::safe_cast<System::Drawing::Image^>(resources->GetObject(L"openButton.BackgroundImage")));
			this->openButton->BackgroundImageLayout = System::Windows::Forms::ImageLayout::Stretch;
			this->openButton->FlatAppearance->BorderSize = 0;
			this->openButton->FlatStyle = System::Windows::Forms::FlatStyle::Flat;
			this->openButton->Location = System::Drawing::Point(6, 2);
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
			this->saveButton->Location = System::Drawing::Point(59, 2);
			this->saveButton->Name = L"saveButton";
			this->saveButton->Size = System::Drawing::Size(40, 40);
			this->saveButton->TabIndex = 5;
			this->saveButton->UseVisualStyleBackColor = false;
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
			this->aboutButton->Location = System::Drawing::Point(255, 3);
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
			this->panel2->Location = System::Drawing::Point(52, 4);
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
			this->saveAsButton->Location = System::Drawing::Point(104, 2);
			this->saveAsButton->Name = L"saveAsButton";
			this->saveAsButton->Size = System::Drawing::Size(40, 40);
			this->saveAsButton->TabIndex = 8;
			this->saveAsButton->UseVisualStyleBackColor = false;
			this->saveAsButton->MouseEnter += gcnew System::EventHandler(this, &HumongousEditorForm::buttonHover);
			this->saveAsButton->MouseLeave += gcnew System::EventHandler(this, &HumongousEditorForm::buttonExit);
			// 
			// panel3
			// 
			this->panel3->BackColor = System::Drawing::SystemColors::ButtonShadow;
			this->panel3->Location = System::Drawing::Point(195, 4);
			this->panel3->Name = L"panel3";
			this->panel3->Size = System::Drawing::Size(1, 36);
			this->panel3->TabIndex = 9;
			// 
			// panel4
			// 
			this->panel4->BackColor = System::Drawing::SystemColors::ButtonShadow;
			this->panel4->Location = System::Drawing::Point(248, 4);
			this->panel4->Name = L"panel4";
			this->panel4->Size = System::Drawing::Size(1, 36);
			this->panel4->TabIndex = 9;
			// 
			// encryptButton
			// 
			this->encryptButton->BackColor = System::Drawing::Color::Transparent;
			this->encryptButton->BackgroundImage = (cli::safe_cast<System::Drawing::Image^>(resources->GetObject(L"encryptButton.BackgroundImage")));
			this->encryptButton->BackgroundImageLayout = System::Windows::Forms::ImageLayout::Stretch;
			this->encryptButton->FlatAppearance->BorderSize = 0;
			this->encryptButton->FlatStyle = System::Windows::Forms::FlatStyle::Flat;
			this->encryptButton->ForeColor = System::Drawing::Color::Transparent;
			this->encryptButton->Location = System::Drawing::Point(202, 2);
			this->encryptButton->Name = L"encryptButton";
			this->encryptButton->Size = System::Drawing::Size(40, 40);
			this->encryptButton->TabIndex = 10;
			this->encryptButton->TabStop = false;
			this->encryptButton->UseVisualStyleBackColor = false;
			this->encryptButton->Click += gcnew System::EventHandler(this, &HumongousEditorForm::encryptButton_Click);
			this->encryptButton->MouseEnter += gcnew System::EventHandler(this, &HumongousEditorForm::buttonHover);
			this->encryptButton->MouseLeave += gcnew System::EventHandler(this, &HumongousEditorForm::buttonExit);
			// 
			// actionPanel
			// 
			this->actionPanel->BackColor = System::Drawing::SystemColors::GradientInactiveCaption;
			this->actionPanel->Controls->Add(this->indexButton);
			this->actionPanel->Controls->Add(this->encryptButton);
			this->actionPanel->Controls->Add(this->panel3);
			this->actionPanel->Controls->Add(this->panel4);
			this->actionPanel->Controls->Add(this->saveAsButton);
			this->actionPanel->Controls->Add(this->panel2);
			this->actionPanel->Controls->Add(this->aboutButton);
			this->actionPanel->Controls->Add(this->saveButton);
			this->actionPanel->Controls->Add(this->openButton);
			this->actionPanel->Dock = System::Windows::Forms::DockStyle::Top;
			this->actionPanel->Location = System::Drawing::Point(0, 24);
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
			this->indexButton->Location = System::Drawing::Point(149, 2);
			this->indexButton->Name = L"indexButton";
			this->indexButton->Size = System::Drawing::Size(40, 40);
			this->indexButton->TabIndex = 11;
			this->indexButton->TabStop = false;
			this->indexButton->UseVisualStyleBackColor = false;
			this->indexButton->Click += gcnew System::EventHandler(this, &HumongousEditorForm::optionIndex_Click);
			this->indexButton->MouseEnter += gcnew System::EventHandler(this, &HumongousEditorForm::buttonHover);
			this->indexButton->MouseLeave += gcnew System::EventHandler(this, &HumongousEditorForm::buttonExit);
			// 
			// HumongousEditorForm
			// 
			this->ClientSize = System::Drawing::Size(991, 561);
			this->Controls->Add(this->actionPanel);
			this->Controls->Add(this->statusStrip2);
			this->Controls->Add(this->splitContainer);
			this->Controls->Add(this->topMenu);
			this->MainMenuStrip = this->topMenu;
			this->Name = L"HumongousEditorForm";
			this->splitContainer->Panel1->ResumeLayout(false);
			this->splitContainer->Panel2->ResumeLayout(false);
			(cli::safe_cast<System::ComponentModel::ISupportInitialize^>(this->splitContainer))->EndInit();
			this->splitContainer->ResumeLayout(false);
			this->treeActionPanel->ResumeLayout(false);
			this->tabControl1->ResumeLayout(false);
			this->topMenu->ResumeLayout(false);
			this->topMenu->PerformLayout();
			this->statusStrip2->ResumeLayout(false);
			this->statusStrip2->PerformLayout();
			this->actionPanel->ResumeLayout(false);
			this->ResumeLayout(false);
			this->PerformLayout();

		}
#pragma endregion
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
						Humongous Game Files (*.HE0, *.HE2, *.HE3, *.HE4, *.(A))\
						\0*.HE0;*.he0;*.HE2;*.he2;*.HE3;*.he3;*.HE4;*.he4;*.(A);*.(a)\0\
						Humongous A Files (*.(A))\
						\0*.(A);*.(a)\0\
						Humongous Talkie Files (*.HE2)\
						\0*.HE2;*.he2\0\
						Humongous Song Files (*.HE4)\
						\0*.HE4;*.he4\0";
		}

#pragma region entryview
		/*
			* Entry view functions.
		*/
		// Checks all entry nodes in the EntryView.
		System::Void selectAllButton_Click(System::Object^ sender, System::EventArgs^ e)
		{
			for (size_t i = 0; i < entryView->Nodes->Count; i++)
			{
				entryView->Nodes[i]->Checked = true;
				exportSelected->Enabled = true;
			}
		}
		// Unchecks all entry nodes in the EntryView.
		System::Void unselectAllButton_Click(System::Object^ sender, System::EventArgs^ e)
		{
			for (size_t i = 0; i < entryView->Nodes->Count; i++)
				entryView->Nodes[i]->Checked = false;

			exportSelected->Enabled = false;
		}
#pragma endregion

		// Opens a window with info about the application (via the top menu or info button).
		System::Void optionIndex_Click(System::Object^ sender, System::EventArgs^ e)
		{
			chunk_reader::FileIndexer f = chunk_reader::FileIndexer();
			if (f.Read())
			{
				System::Windows::Forms::MessageBox::Show("Successfully indexed file.", "Success", System::Windows::Forms::MessageBoxButtons::OK, System::Windows::Forms::MessageBoxIcon::Information);
				toolProgressBar->Value = 0;
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
					toolProgressBar->Value = 0;
					return;
				}

				delete[] path;
			}
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
	public:
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
		System::Void encryptButton_Click(System::Object^ sender, System::EventArgs^ e)
		{
			chunk_reader::FileDecrypter fileDecrypter;
			if (fileDecrypter.Read())
			{
				System::Windows::Forms::MessageBox::Show("Successfully decrypted/encrypted file.", "Success", System::Windows::Forms::MessageBoxButtons::OK, System::Windows::Forms::MessageBoxIcon::Information);
				toolProgressBar->Value = 0;
				return;
			}
		}
	};
}