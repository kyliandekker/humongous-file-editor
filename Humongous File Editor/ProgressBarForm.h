#pragma once

namespace HumongousFileEditor
{

	using namespace System;
	using namespace System::ComponentModel;
	using namespace System::Collections;
	using namespace System::Windows::Forms;
	using namespace System::Data;
	using namespace System::Drawing;

	/// <summary>
	/// Summary for ProgressBarForm
	/// </summary>
	public ref class ProgressBarForm : public System::Windows::Forms::Form
	{
	public:
		ProgressBarForm(void)
		{
			InitializeComponent();
			//
			//TODO: Add the constructor code here
			//
		}

	protected:
		/// <summary>
		/// Clean up any resources being used.
		/// </summary>
		~ProgressBarForm()
		{
			if (components)
			{
				delete components;
			}
		}
	public: System::Windows::Forms::Label^ actionText;
	public: System::Windows::Forms::Label^ programText;
	public: System::Windows::Forms::ProgressBar^ progressBar1;
	public: System::Windows::Forms::ProgressBar^ progressBar2;
	protected:

	private:
		/// <summary>
		/// Required designer variable.
		/// </summary>
		System::ComponentModel::Container ^components;

#pragma region Windows Form Designer generated code
		/// <summary>
		/// Required method for Designer support - do not modify
		/// the contents of this method with the code editor.
		/// </summary>
		void InitializeComponent(void)
		{
			this->actionText = (gcnew System::Windows::Forms::Label());
			this->programText = (gcnew System::Windows::Forms::Label());
			this->progressBar1 = (gcnew System::Windows::Forms::ProgressBar());
			this->progressBar2 = (gcnew System::Windows::Forms::ProgressBar());
			this->SuspendLayout();
			// 
			// actionText
			// 
			this->actionText->AutoSize = true;
			this->actionText->Location = System::Drawing::Point(16, 9);
			this->actionText->Name = L"actionText";
			this->actionText->Size = System::Drawing::Size(114, 20);
			this->actionText->TabIndex = 1;
			this->actionText->Text = L"Compiling HE0";
			this->actionText->UseMnemonic = false;
			// 
			// programText
			// 
			this->programText->AutoSize = true;
			this->programText->Location = System::Drawing::Point(16, 80);
			this->programText->Name = L"programText";
			this->programText->Size = System::Drawing::Size(114, 20);
			this->programText->TabIndex = 3;
			this->programText->Text = L"Compiling HE0";
			this->programText->UseMnemonic = false;
			// 
			// progressBar1
			// 
			this->progressBar1->Location = System::Drawing::Point(16, 43);
			this->progressBar1->Name = L"progressBar1";
			this->progressBar1->Size = System::Drawing::Size(450, 24);
			this->progressBar1->TabIndex = 4;
			// 
			// progressBar2
			// 
			this->progressBar2->Location = System::Drawing::Point(16, 114);
			this->progressBar2->Name = L"progressBar2";
			this->progressBar2->Size = System::Drawing::Size(450, 24);
			this->progressBar2->TabIndex = 5;
			// 
			// ProgressBarForm
			// 
			this->AutoScaleDimensions = System::Drawing::SizeF(9, 20);
			this->AutoScaleMode = System::Windows::Forms::AutoScaleMode::Font;
			this->ClientSize = System::Drawing::Size(478, 155);
			this->Controls->Add(this->progressBar2);
			this->Controls->Add(this->progressBar1);
			this->Controls->Add(this->programText);
			this->Controls->Add(this->actionText);
			this->FormBorderStyle = System::Windows::Forms::FormBorderStyle::FixedDialog;
			this->MaximizeBox = false;
			this->MinimizeBox = false;
			this->Name = L"ProgressBarForm";
			this->ShowIcon = false;
			this->ControlBox = false;
			this->ShowInTaskbar = false;
			this->Text = L"ProgressBarForm";
			this->ResumeLayout(false);
			this->PerformLayout();
		}
#pragma endregion
	};
}
