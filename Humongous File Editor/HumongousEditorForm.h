#pragma once

#include <ShlObj_core.h>
#include <string>
#include <windows.h>
#include <msclr\marshal_cppstd.h>

#include "utils/utils.h"
#include "Humongous/Entry.h"
#include "Humongous/EntryContainer.h"
#include "Humongous/Notes/NoteContainer.h"
#include "Systems/AudioSystem.h"
#include "Humongous/FileType.h"
#include "DesignerExtras.h"
#include <InputForm.h>

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
	private:
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
	public:
		System::Windows::Forms::ToolStripProgressBar^ toolProgressBar;
		System::Windows::Forms::TreeView^ entryView;
	private:
		System::Windows::Forms::Panel^ treeActionPanel;

		System::Windows::Forms::Button^ exportSelected;
		System::Windows::Forms::Button^ unselectAll;
		System::Windows::Forms::Button^ selectAllButton;
		System::Windows::Forms::Button^ openButton;
		System::Windows::Forms::Panel^ panel1;
		System::Windows::Forms::Button^ saveButton;
		System::Windows::Forms::Button^ helpButton;
		System::Windows::Forms::Panel^ panel2;
		System::Windows::Forms::Button^ saveAsButton;
		System::Windows::Forms::Panel^ panel3;
		System::Windows::Forms::ToolStripMenuItem^ optionSave;
		System::Windows::Forms::ToolStripSeparator^ toolStripSeparator1;
		System::Windows::Forms::TabPage^ tabWelcome;
		System::Windows::Forms::Label^ welcomeLabel;


		System::String^ windowTitle = gcnew System::String("Humongous File Editor");
	public:
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
			this->welcomeLabel = (gcnew System::Windows::Forms::Label());
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
			this->panel1 = (gcnew System::Windows::Forms::Panel());
			this->panel3 = (gcnew System::Windows::Forms::Panel());
			this->saveAsButton = (gcnew System::Windows::Forms::Button());
			this->panel2 = (gcnew System::Windows::Forms::Panel());
			this->helpButton = (gcnew System::Windows::Forms::Button());
			this->saveButton = (gcnew System::Windows::Forms::Button());
			(cli::safe_cast<System::ComponentModel::ISupportInitialize^>(this->splitContainer))->BeginInit();
			this->splitContainer->Panel1->SuspendLayout();
			this->splitContainer->Panel2->SuspendLayout();
			this->splitContainer->SuspendLayout();
			this->treeActionPanel->SuspendLayout();
			this->tabControl1->SuspendLayout();
			this->tabWelcome->SuspendLayout();
			this->topMenu->SuspendLayout();
			this->statusStrip2->SuspendLayout();
			this->panel1->SuspendLayout();
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
			this->splitContainer->Size = System::Drawing::Size(943, 409);
			this->splitContainer->SplitterDistance = 536;
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
			this->treeActionPanel->Size = System::Drawing::Size(516, 29);
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
			this->exportSelected->Click += gcnew System::EventHandler(this, &HumongousEditorForm::exportSelected_Click);
			// 
			// unselectAll
			// 
			this->unselectAll->Location = System::Drawing::Point(84, 3);
			this->unselectAll->Name = L"unselectAll";
			this->unselectAll->Size = System::Drawing::Size(75, 23);
			this->unselectAll->TabIndex = 1;
			this->unselectAll->Text = L"Select None";
			this->unselectAll->UseVisualStyleBackColor = true;
			this->unselectAll->Click += gcnew System::EventHandler(this, &HumongousEditorForm::unselectAll_Click);
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
			this->entryView->AllowDrop = true;
			this->entryView->Anchor = static_cast<System::Windows::Forms::AnchorStyles>((((System::Windows::Forms::AnchorStyles::Top | System::Windows::Forms::AnchorStyles::Bottom)
				| System::Windows::Forms::AnchorStyles::Left)
				| System::Windows::Forms::AnchorStyles::Right));
			this->entryView->BackColor = System::Drawing::Color::FromArgb(static_cast<System::Int32>(static_cast<System::Byte>(249)), static_cast<System::Int32>(static_cast<System::Byte>(249)),
				static_cast<System::Int32>(static_cast<System::Byte>(249)));
			this->entryView->CheckBoxes = true;
			this->entryView->ItemHeight = 20;
			this->entryView->Location = System::Drawing::Point(10, 45);
			this->entryView->Name = L"entryView";
			this->entryView->Size = System::Drawing::Size(516, 352);
			this->entryView->TabIndex = 0;
			this->entryView->AfterCheck += gcnew System::Windows::Forms::TreeViewEventHandler(this, &HumongousEditorForm::entryView_Check);
			this->entryView->NodeMouseClick += gcnew System::Windows::Forms::TreeNodeMouseClickEventHandler(this, &HumongousEditorForm::entryView_Click);
			this->entryView->MouseDoubleClick += gcnew System::Windows::Forms::MouseEventHandler(this, &HumongousEditorForm::entryView_DoubleClick);
			// 
			// tabControl1
			// 
			this->tabControl1->AllowDrop = true;
			this->tabControl1->Controls->Add(this->tabWelcome);
			this->tabControl1->Dock = System::Windows::Forms::DockStyle::Fill;
			this->tabControl1->ImeMode = System::Windows::Forms::ImeMode::NoControl;
			this->tabControl1->Location = System::Drawing::Point(10, 10);
			this->tabControl1->Name = L"tabControl1";
			this->tabControl1->SelectedIndex = 0;
			this->tabControl1->Size = System::Drawing::Size(383, 389);
			this->tabControl1->TabIndex = 0;
			// 
			// tabWelcome
			// 
			this->tabWelcome->Controls->Add(this->welcomeLabel);
			this->tabWelcome->Location = System::Drawing::Point(4, 25);
			this->tabWelcome->Name = L"tabWelcome";
			this->tabWelcome->Size = System::Drawing::Size(375, 360);
			this->tabWelcome->TabIndex = 0;
			this->tabWelcome->Text = L"Welcome!";
			this->tabWelcome->UseVisualStyleBackColor = true;
			// 
			// welcomeLabel
			// 
			this->welcomeLabel->AutoSize = true;
			this->welcomeLabel->Dock = System::Windows::Forms::DockStyle::Top;
			this->welcomeLabel->Location = System::Drawing::Point(0, 0);
			this->welcomeLabel->Name = L"welcomeLabel";
			this->welcomeLabel->Padding = System::Windows::Forms::Padding(20);
			this->welcomeLabel->Size = System::Drawing::Size(270, 56);
			this->welcomeLabel->TabIndex = 0;
			this->welcomeLabel->Text = L"Open a Humongous file to get started!";
			// 
			// optionSeperator1
			// 
			this->optionSeperator1->DropDownItems->AddRange(gcnew cli::array< System::Windows::Forms::ToolStripItem^  >(6) {
				this->optionOpen,
					this->toolStripSeparator1, this->optionSave, this->optionSaveAs, this->toolStripSeparator2, this->optionQuit
			});
			this->optionSeperator1->Name = L"optionSeperator1";
			this->optionSeperator1->Size = System::Drawing::Size(51, 27);
			this->optionSeperator1->Text = L"File";
			// 
			// optionOpen
			// 
			this->optionOpen->Name = L"optionOpen";
			this->optionOpen->ShortcutKeys = static_cast<System::Windows::Forms::Keys>((System::Windows::Forms::Keys::Control | System::Windows::Forms::Keys::O));
			this->optionOpen->Size = System::Drawing::Size(266, 30);
			this->optionOpen->Text = L"Open";
			this->optionOpen->Click += gcnew System::EventHandler(this, &HumongousEditorForm::optionOpen_Click);
			// 
			// toolStripSeparator1
			// 
			this->toolStripSeparator1->Name = L"toolStripSeparator1";
			this->toolStripSeparator1->Size = System::Drawing::Size(263, 6);
			// 
			// optionSave
			// 
			this->optionSave->Enabled = false;
			this->optionSave->Name = L"optionSave";
			this->optionSave->ShortcutKeys = static_cast<System::Windows::Forms::Keys>((System::Windows::Forms::Keys::Control | System::Windows::Forms::Keys::S));
			this->optionSave->Size = System::Drawing::Size(266, 30);
			this->optionSave->Text = L"Save";
			// 
			// optionSaveAs
			// 
			this->optionSaveAs->Enabled = false;
			this->optionSaveAs->Name = L"optionSaveAs";
			this->optionSaveAs->ShortcutKeys = static_cast<System::Windows::Forms::Keys>(((System::Windows::Forms::Keys::Control | System::Windows::Forms::Keys::Shift)
				| System::Windows::Forms::Keys::S));
			this->optionSaveAs->Size = System::Drawing::Size(266, 30);
			this->optionSaveAs->Text = L"Save As";
			this->optionSaveAs->Click += gcnew System::EventHandler(this, &HumongousEditorForm::optionSave_Click);
			// 
			// toolStripSeparator2
			// 
			this->toolStripSeparator2->Name = L"toolStripSeparator2";
			this->toolStripSeparator2->Size = System::Drawing::Size(263, 6);
			// 
			// optionQuit
			// 
			this->optionQuit->Name = L"optionQuit";
			this->optionQuit->ShortcutKeys = static_cast<System::Windows::Forms::Keys>((System::Windows::Forms::Keys::Control | System::Windows::Forms::Keys::Q));
			this->optionQuit->Size = System::Drawing::Size(266, 30);
			this->optionQuit->Text = L"Exit";
			// 
			// helpMenu
			// 
			this->helpMenu->DropDownItems->AddRange(gcnew cli::array< System::Windows::Forms::ToolStripItem^  >(1) { this->optionAbout });
			this->helpMenu->Name = L"helpMenu";
			this->helpMenu->Size = System::Drawing::Size(61, 27);
			this->helpMenu->Text = L"Help";
			// 
			// optionAbout
			// 
			this->optionAbout->Name = L"optionAbout";
			this->optionAbout->Size = System::Drawing::Size(151, 30);
			this->optionAbout->Text = L"About";
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
			this->topMenu->Size = System::Drawing::Size(943, 31);
			this->topMenu->TabIndex = 0;
			this->topMenu->Text = L"menuStrip2";
			// 
			// statusStrip2
			// 
			this->statusStrip2->ImageScalingSize = System::Drawing::Size(22, 22);
			this->statusStrip2->Items->AddRange(gcnew cli::array< System::Windows::Forms::ToolStripItem^  >(1) { this->toolProgressBar });
			this->statusStrip2->Location = System::Drawing::Point(0, 477);
			this->statusStrip2->Margin = System::Windows::Forms::Padding(5);
			this->statusStrip2->Name = L"statusStrip2";
			this->statusStrip2->Size = System::Drawing::Size(943, 27);
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
			// panel1
			// 
			this->panel1->BackColor = System::Drawing::SystemColors::GradientInactiveCaption;
			this->panel1->Controls->Add(this->panel3);
			this->panel1->Controls->Add(this->saveAsButton);
			this->panel1->Controls->Add(this->panel2);
			this->panel1->Controls->Add(this->helpButton);
			this->panel1->Controls->Add(this->saveButton);
			this->panel1->Controls->Add(this->openButton);
			this->panel1->Dock = System::Windows::Forms::DockStyle::Top;
			this->panel1->Location = System::Drawing::Point(0, 31);
			this->panel1->Name = L"panel1";
			this->panel1->Padding = System::Windows::Forms::Padding(10);
			this->panel1->Size = System::Drawing::Size(943, 44);
			this->panel1->TabIndex = 4;
			// 
			// panel3
			// 
			this->panel3->BackColor = System::Drawing::SystemColors::ButtonShadow;
			this->panel3->Location = System::Drawing::Point(150, 4);
			this->panel3->Name = L"panel3";
			this->panel3->Size = System::Drawing::Size(1, 36);
			this->panel3->TabIndex = 9;
			// 
			// saveAsButton
			// 
			this->saveAsButton->BackColor = System::Drawing::Color::Transparent;
			this->saveAsButton->BackgroundImage = (cli::safe_cast<System::Drawing::Image^>(resources->GetObject(L"saveAsButton.BackgroundImage")));
			this->saveAsButton->BackgroundImageLayout = System::Windows::Forms::ImageLayout::Stretch;
			this->saveAsButton->FlatAppearance->BorderSize = 0;
			this->saveAsButton->FlatStyle = System::Windows::Forms::FlatStyle::Flat;
			this->saveAsButton->Location = System::Drawing::Point(104, 2);
			this->saveAsButton->Name = L"saveAsButton";
			this->saveAsButton->Size = System::Drawing::Size(40, 40);
			this->saveAsButton->TabIndex = 8;
			this->saveAsButton->UseVisualStyleBackColor = false;
			this->saveAsButton->Click += gcnew System::EventHandler(this, &HumongousEditorForm::optionSave_Click);
			this->saveAsButton->MouseEnter += gcnew System::EventHandler(this, &HumongousEditorForm::buttonHover);
			this->saveAsButton->MouseLeave += gcnew System::EventHandler(this, &HumongousEditorForm::buttonExit);
			// 
			// panel2
			// 
			this->panel2->BackColor = System::Drawing::SystemColors::ButtonShadow;
			this->panel2->Location = System::Drawing::Point(52, 4);
			this->panel2->Name = L"panel2";
			this->panel2->Size = System::Drawing::Size(1, 36);
			this->panel2->TabIndex = 7;
			// 
			// helpButton
			// 
			this->helpButton->BackColor = System::Drawing::Color::Transparent;
			this->helpButton->BackgroundImage = (cli::safe_cast<System::Drawing::Image^>(resources->GetObject(L"helpButton.BackgroundImage")));
			this->helpButton->BackgroundImageLayout = System::Windows::Forms::ImageLayout::Stretch;
			this->helpButton->FlatAppearance->BorderSize = 0;
			this->helpButton->FlatStyle = System::Windows::Forms::FlatStyle::Flat;
			this->helpButton->ForeColor = System::Drawing::Color::Transparent;
			this->helpButton->Location = System::Drawing::Point(157, 2);
			this->helpButton->Name = L"helpButton";
			this->helpButton->Size = System::Drawing::Size(40, 40);
			this->helpButton->TabIndex = 6;
			this->helpButton->UseVisualStyleBackColor = false;
			this->helpButton->Click += gcnew System::EventHandler(this, &HumongousEditorForm::optionAbout_Click);
			this->helpButton->MouseEnter += gcnew System::EventHandler(this, &HumongousEditorForm::buttonHover);
			this->helpButton->MouseLeave += gcnew System::EventHandler(this, &HumongousEditorForm::buttonExit);
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
			// HumongousEditorForm
			// 
			this->ClientSize = System::Drawing::Size(943, 504);
			this->Controls->Add(this->panel1);
			this->Controls->Add(this->statusStrip2);
			this->Controls->Add(this->splitContainer);
			this->Controls->Add(this->topMenu);
			this->MainMenuStrip = this->topMenu;
			this->Name = L"HumongousEditorForm";
			this->Load += gcnew System::EventHandler(this, &HumongousEditorForm::MyForm_Load);
			this->splitContainer->Panel1->ResumeLayout(false);
			this->splitContainer->Panel2->ResumeLayout(false);
			(cli::safe_cast<System::ComponentModel::ISupportInitialize^>(this->splitContainer))->EndInit();
			this->splitContainer->ResumeLayout(false);
			this->treeActionPanel->ResumeLayout(false);
			this->tabControl1->ResumeLayout(false);
			this->tabWelcome->ResumeLayout(false);
			this->tabWelcome->PerformLayout();
			this->topMenu->ResumeLayout(false);
			this->topMenu->PerformLayout();
			this->statusStrip2->ResumeLayout(false);
			this->statusStrip2->PerformLayout();
			this->panel1->ResumeLayout(false);
			this->ResumeLayout(false);
			this->PerformLayout();

		}
#pragma endregion
	private:
		System::Void MyForm_Load(System::Object^ sender, System::EventArgs^ e)
		{
			this->Text = windowTitle;
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
		// Opens a file (via the top menu or the load button).
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

				std::string pathString = std::string(path);
				std::string extension = pathString.substr(pathString.find_last_of(".") + 1);

				// Set the title of the window.
				System::String^ title = windowTitle + gcnew System::String(" - [") + gcnew System::String(pathString.c_str()) + gcnew System::String("]");
				this->Text = title;

				// Clear all previously loaded nodes.
				this->entryView->Nodes->Clear();
				for (size_t i = this->tabControl1->TabPages->Count - 1; i > 0; i--)
					this->tabControl1->TabPages->RemoveAt(i);

				entryContainer.Decompile(path);

				delete[] path;
			}
		}
		// Saves a file (via the top menu or the save button).
		System::Void optionSave_Click(System::Object^ sender, System::EventArgs^ e)
		{
			OPENFILENAME ofn;

			ZeroMemory(&ofn, sizeof(ofn));
			ofn.lStructSize = sizeof(ofn);

			wchar_t wtext[260];
			ZeroMemory(wtext, 260);
			mbstowcs(wtext, entryContainer.fileName.c_str(), entryContainer.fileName.length());
			ofn.lpstrFile = wtext;
			ofn.nMaxFile = MAX_PATH;

			std::string pathString = std::string(entryContainer.fileName);

			ofn.lpstrFilter = getFilter();
			ofn.nFilterIndex = getFileTypeByExtension(pathString);
			ofn.lpstrFileTitle = nullptr;
			ofn.nMaxFileTitle = 0;
			ofn.lpstrInitialDir = nullptr;
			ofn.Flags = OFN_PATHMUSTEXIST | OFN_FILEMUSTEXIST;

			if (GetSaveFileNameW(&ofn))
			{
				const auto path = new char[wcslen(ofn.lpstrFile) + 1];
				wsprintfA(path, "%S", ofn.lpstrFile);

				entryContainer.Compile(path);

				delete[] path;
			}
		}
		// Opens a window with info about the application (via the top menu or info button).
		System::Void optionAbout_Click(System::Object^ sender, System::EventArgs^ e)
		{
			MessageBox::Show("Humongous File Editor by Kylian Dekker\nVersion: 0.0.1\n\nMany thanks to rzil for helping me with reading the resource files.", "About", MessageBoxButtons::OK, MessageBoxIcon::Information);
		}

		/*
			* EntryView Node Callbacks.
		*/

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
				AddTab(node, entryContainer[node->num]);
			tabControl1->SelectedIndex = tabControl1->Controls->IndexOfKey(node->Name);
		}
		// Right click on a node.
		System::Void entryView_Click(System::Object^ sender, System::Windows::Forms::TreeNodeMouseClickEventArgs^ e)
		{
			if (e->Button == System::Windows::Forms::MouseButtons::Right)
			{
				System::Windows::Forms::TreeView^ view = static_cast<System::Windows::Forms::TreeView^>(sender);
				view->SelectedNode = e->Node;

				System::Windows::Forms::ContextMenuStrip^ menu = gcnew System::Windows::Forms::ContextMenuStrip();
				
				HumongousToolStripMenuItem^ exportItem = gcnew HumongousToolStripMenuItem();
				exportItem->Click += gcnew System::EventHandler(this, &HumongousEditorForm::menuClick);
				exportItem->Name = "Export";
				exportItem->Text = "Export";
				exportItem->node = static_cast<HumongousNode^>(e->Node);
				menu->Items->Add(exportItem);

				ToolStripSeparator^ separator1 = gcnew ToolStripSeparator();
				separator1->Name = L"toolStripSeparator1";
				separator1->Size = System::Drawing::Size(263, 6);
				menu->Items->Add(separator1);
				
				HumongousToolStripMenuItem^ addNoteItem = gcnew HumongousToolStripMenuItem();
				addNoteItem->Click += gcnew System::EventHandler(this, &HumongousEditorForm::menuClick);
				addNoteItem->Name = "Add Note";
				addNoteItem->Text = "Add Note";
				addNoteItem->node = static_cast<HumongousNode^>(e->Node);
				menu->Items->Add(addNoteItem);

				ToolStripSeparator^ separator2 = gcnew ToolStripSeparator();
				separator2->Name = L"toolStripSeparator1";
				separator2->Size = System::Drawing::Size(263, 6);
				menu->Items->Add(separator2);

				HumongousToolStripMenuItem^ replaceItem = gcnew HumongousToolStripMenuItem();
				replaceItem->Click += gcnew System::EventHandler(this, &HumongousEditorForm::menuClick);
				replaceItem->Name = "Replace";
				replaceItem->Text = "Replace";
				replaceItem->node = static_cast<HumongousNode^>(e->Node);
				menu->Items->Add(replaceItem);

				ToolStripSeparator^ separator3 = gcnew ToolStripSeparator();
				separator3->Name = L"toolStripSeparator2";
				separator3->Size = System::Drawing::Size(263, 6);
				menu->Items->Add(separator3);

				HumongousToolStripMenuItem^ exitItem = gcnew HumongousToolStripMenuItem();
				exitItem->Click += gcnew System::EventHandler(this, &HumongousEditorForm::menuClick);
				exitItem->Name = "Exit";
				exitItem->Text = "Exit";
				exitItem->node = static_cast<HumongousNode^>(e->Node);
				menu->Items->Add(exitItem);

				menu->Show(this, e->Location);
				menu->Show(System::Windows::Forms::Cursor::Position);
			}
		}
		// Right click menu for the nodes.
		System::Void menuClick(System::Object^ sender, System::EventArgs^ e)
		{
			HumongousToolStripMenuItem^ item = static_cast<HumongousToolStripMenuItem^>(sender);
			if (item->Name == "Export")
			{
				SaveEntry(item->node->num);
			}
			else if (item->Name == "Replace")
			{
				ReplaceEntry(item->node->num);
			}
			else if (item->Name == "Add Note")
			{
				InputForm^ form = gcnew InputForm();
				System::Windows::Forms::DialogResult dialogResult = form->ShowDialog();
				if (dialogResult == System::Windows::Forms::DialogResult::OK)
				{
					System::String^ value = form->textBox1->Text;

					std::string standardString = msclr::interop::marshal_as<std::string>(value);
					notes::noteContainer.AddNote(standardString.c_str(), item->node->num);
					notes::noteContainer.SaveNotes(entryContainer.filePath);
					item->node->Text = item->node->Name + gcnew System::String(" (") + notes::noteContainer.GetNote(item->node->num) + gcnew System::String(")");
				}
			}
		}

		/*
			* Info tab methods.
		*/

		// Adds a row of info to the info tab.
		void AddInfoRow(System::String^ key, System::String^ value, System::Windows::Forms::TabPage^ tab, float& posX, float& posY)
		{
			System::Windows::Forms::Label^ keyLabel;
			keyLabel = (gcnew System::Windows::Forms::Label());
			keyLabel->SuspendLayout();

			System::Windows::Forms::Label^ valueLabel;
			valueLabel = (gcnew System::Windows::Forms::Label());
			valueLabel->SuspendLayout();

			keyLabel->Anchor = static_cast<System::Windows::Forms::AnchorStyles>(((System::Windows::Forms::AnchorStyles::Top | System::Windows::Forms::AnchorStyles::Left)
				| System::Windows::Forms::AnchorStyles::Right));
			keyLabel->AutoSize = true;
			keyLabel->Location = System::Drawing::Point(posX, posY);
			keyLabel->Name = key;
			keyLabel->Size = System::Drawing::Size(35, 13);
			keyLabel->TabIndex = 0;
			keyLabel->Text = key;
			keyLabel->TextAlign = System::Drawing::ContentAlignment::MiddleCenter;

			valueLabel->Anchor = static_cast<System::Windows::Forms::AnchorStyles>(((System::Windows::Forms::AnchorStyles::Top | System::Windows::Forms::AnchorStyles::Left)
				| System::Windows::Forms::AnchorStyles::Right));
			valueLabel->AutoSize = true;
			valueLabel->Location = System::Drawing::Point(posX + 100, posY);
			valueLabel->Name = value;
			valueLabel->Size = System::Drawing::Size(35, 13);
			valueLabel->TabIndex = 1;
			valueLabel->Text = value;
			valueLabel->TextAlign = System::Drawing::ContentAlignment::MiddleCenter;

			tab->Controls->Add(keyLabel);
			tab->Controls->Add(valueLabel);

			posY += 40;
		}

		// Adds buttons to the info tab.
		void AddSoundButtons(System::Windows::Forms::Panel^ tab, SongEntry* songEntry, int i)
		{
			HumongousButton^ playButton;
			playButton = (gcnew HumongousButton());

			playButton->Location = System::Drawing::Point(232, 53);
			playButton->Name = gcnew System::String(std::to_string(i).c_str());
			playButton->Size = System::Drawing::Size(75, 23);
			playButton->TabIndex = 2;
			playButton->Text = L"Play";
			playButton->num = i;
			playButton->UseVisualStyleBackColor = true;
			playButton->Click += gcnew System::EventHandler(this, &HumongousEditorForm::playButton_Click);

			HumongousButton^ stopButton;
			stopButton = (gcnew HumongousButton());

			stopButton->Location = System::Drawing::Point(232, 53);
			stopButton->Name = gcnew System::String(std::to_string(i).c_str());
			stopButton->Size = System::Drawing::Size(75, 23);
			stopButton->TabIndex = 2;
			stopButton->Text = L"Stop";
			stopButton->num = i;
			stopButton->UseVisualStyleBackColor = true;
			stopButton->Click += gcnew System::EventHandler(this, &HumongousEditorForm::stopButton_Click);

			playButton->ResumeLayout(false);
			stopButton->ResumeLayout(false);
			tab->Controls->Add(playButton);
			tab->Controls->Add(stopButton);
		}
		// Callback for the play button.
		System::Void playButton_Click(System::Object^ sender, System::EventArgs^ e)
		{
			HumongousButton^ btn = (HumongousButton^)sender;
			SongEntry* soundEntry = static_cast<SongEntry*>(entryContainer[btn->num]);
			audioSystem.Play(soundEntry->data, soundEntry->size);
		}
		// Callback for the stop button.
		System::Void stopButton_Click(System::Object^ sender, System::EventArgs^ e)
		{
			audioSystem.Stop();
		}
	public:
		/// <summary>
		/// Adds a new tab to the tab control.
		/// </summary>
		/// <param name="node">The original node that was clicked on.</param>
		/// <param name="entry">The entry.</param>
		void AddTab(HumongousNode^ node, Entry* entry)
		{
			System::Windows::Forms::TabPage^ newTab;
			newTab = (gcnew System::Windows::Forms::TabPage());
			newTab->SuspendLayout();

			newTab = (gcnew System::Windows::Forms::TabPage());
			newTab->Location = System::Drawing::Point(4, 25);
			newTab->Name = node->Name;
			newTab->Padding = System::Windows::Forms::Padding(10, 3, 3, 3);
			newTab->Size = System::Drawing::Size(659, 396);
			newTab->TabIndex = 0;
			newTab->Text = node->Name;
			newTab->UseVisualStyleBackColor = true;

			System::Windows::Forms::Panel^ actionPanel;
			actionPanel = (gcnew System::Windows::Forms::TableLayoutPanel());

			actionPanel->Dock = System::Windows::Forms::DockStyle::Bottom;
			actionPanel->Location = System::Drawing::Point(3, 306);
			actionPanel->Name = L"Action Panel";
			actionPanel->Size = System::Drawing::Size(418, 87);
			actionPanel->TabIndex = 1;

			std::string type = EntryTypeToString(entry->fileType);

			float posX = 35, posY = 35;
			AddInfoRow("Name", node->Text, newTab, posX, posY);
			AddInfoRow("Type", gcnew System::String(type.c_str()), newTab, posX, posY);
			switch (entry->fileType)
			{
				case EntryType::EntryType_Song:
				{
					SongEntry* songEntry = static_cast<SongEntry*>(entry);
					AddInfoRow("Sample Rate", gcnew System::String(std::to_string(songEntry->sample_rate).c_str()), newTab, posX, posY);
					AddInfoRow("Size (in bytes)", gcnew System::String(std::to_string(songEntry->size).c_str()), newTab, posX, posY);
					AddSoundButtons(actionPanel, songEntry, entry->num);
					break;
				}
				case EntryType::EntryType_Talkie:
				{
					TalkieEntry* talkieEntry = static_cast<TalkieEntry*>(entry);
					AddInfoRow("Sample Rate", gcnew System::String(std::to_string(talkieEntry->sample_rate).c_str()), newTab, posX, posY);
					AddInfoRow("Size (in bytes)", gcnew System::String(std::to_string(talkieEntry->size).c_str()), newTab, posX, posY);
					AddInfoRow("SBNG", gcnew System::String(talkieEntry->sbng_size == 0 ? "true" : "false"), newTab, posX, posY);
					AddSoundButtons(actionPanel, talkieEntry, entry->num);
					type = "Sound";
					break;
				}
			}

			HumongousButton^ exportButton;
			exportButton = (gcnew HumongousButton());

			exportButton->Location = System::Drawing::Point(232, 53);
			exportButton->Name = gcnew System::String(std::to_string(entry->num).c_str());
			exportButton->Size = System::Drawing::Size(75, 23);
			exportButton->TabIndex = 2;
			exportButton->Text = L"Export";
			exportButton->num = entry->num;
			exportButton->UseVisualStyleBackColor = true;
			exportButton->Click += gcnew System::EventHandler(this, &HumongousEditorForm::exportButton_Click);

			exportButton->ResumeLayout(false);
			actionPanel->Controls->Add(exportButton);

			newTab->ResumeLayout(false);

			newTab->Controls->Add(actionPanel);
			tabControl1->Controls->Add(newTab);
		}
	private:
		// Callback for the export button.
		System::Void exportButton_Click(System::Object^ sender, System::EventArgs^ e)
		{
			HumongousButton^ btn = (HumongousButton^)sender;
			SaveEntry(btn->num);
		}
		// Enables or disables the export button when checking or unchecking a node in the EntryView.
		System::Void entryView_Check(System::Object^ sender, System::Windows::Forms::TreeViewEventArgs^ e)
		{
			for (size_t i = 0; i < entryView->Nodes->Count; i++)
				if (entryView->Nodes[i]->Checked)
				{
					exportSelected->Enabled = true;
					return;
				}

			exportSelected->Enabled = false;
		}
		// Saves an entry.
		void SaveEntry(int num)
		{
			Entry* entry = entryContainer[num];
			entry->OpenSave();
		}
		// Replaces an entry.
		void ReplaceEntry(int num)
		{
			Entry* entry = entryContainer[num];
			entry->OpenReplace();
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
		System::Void unselectAll_Click(System::Object^ sender, System::EventArgs^ e)
		{
			for (size_t i = 0; i < entryView->Nodes->Count; i++)
				entryView->Nodes[i]->Checked = false;

			exportSelected->Enabled = false;
		}
		// Callback for exporting all checked entry nodes in the EntryView to a directory.
		System::Void exportSelected_Click(System::Object^ sender, System::EventArgs^ e)
		{
			std::string strDirectory;
			TCHAR szDir[MAX_PATH];
			BROWSEINFO bInfo;
			bInfo.pidlRoot = NULL;
			bInfo.pszDisplayName = szDir; // Address of a buffer to receive the display name of the folder selected by the user
			bInfo.lpszTitle = L"Please, select a directory"; // Title of the dialog
			bInfo.ulFlags = BIF_USENEWUI;
			bInfo.lpfn = NULL;
			bInfo.lParam = 0;
			bInfo.iImage = -1;

			LPITEMIDLIST lpItem = SHBrowseForFolder(&bInfo);
			if (lpItem != NULL)
			{
				SHGetPathFromIDList(lpItem, szDir);
				std::wstring wStr = szDir;
				strDirectory = std::string(wStr.begin(), wStr.end());

				std::vector<Entry*> entries;
				for (size_t i = 0; i < entryView->Nodes->Count; i++)
				{
					if (entryView->Nodes[i]->Checked)
					{
						HumongousNode^ node = static_cast<HumongousNode^>(entryView->Nodes[i]);
						entries.push_back(entryContainer[node->num]);
					}
				}
				ExportSelected(strDirectory, entries);
			}
		}
		// Exports a list of entries to a directory.
		void ExportSelected(std::string directory, std::vector<Entry*> entries)
		{
			for (size_t i = 0; i < entries.size(); i++)
			{
				std::string path = directory + "\\" + entries[i]->GetCommonExtension();
				entries[i]->Save(path);
			}
		}
	};
}