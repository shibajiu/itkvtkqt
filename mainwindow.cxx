#include "mainwindow.h"
#include "ui_mainwindow.h"

// Define own interaction style
class myVtkInteractorStyleImage : public vtkInteractorStyleImage
{
public:
	static myVtkInteractorStyleImage* New();
	vtkTypeMacro(myVtkInteractorStyleImage, vtkInteractorStyleImage);

protected:
	vtkImageViewer2* _ImageViewer;
	vtkTextMapper* _StatusMapper;
	int _Slice;
	int _MinSlice;
	int _MaxSlice;

public:
	void SetImageViewer(vtkImageViewer2* imageViewer) {
		_ImageViewer = imageViewer;
		_MinSlice = imageViewer->GetSliceMin();
		_MaxSlice = imageViewer->GetSliceMax();
		_Slice = _MinSlice;
		cout << "Slicer: Min = " << _MinSlice << ", Max = " << _MaxSlice << std::endl;
	}

	void SetStatusMapper(vtkTextMapper* statusMapper) {
		_StatusMapper = statusMapper;
	}


protected:
	void MoveSliceForward() {
		if (_Slice < _MaxSlice) {
			_Slice += 1;
			cout << "MoveSliceForward::Slice = " << _Slice << std::endl;
			_ImageViewer->SetSlice(_Slice);
			std::string msg = StatusMessage::Format(_Slice, _MaxSlice);
			_StatusMapper->SetInput(msg.c_str());
			_ImageViewer->Render();
		}
	}

	void MoveSliceBackward() {
		if (_Slice > _MinSlice) {
			_Slice -= 1;
			cout << "MoveSliceBackward::Slice = " << _Slice << std::endl;
			_ImageViewer->SetSlice(_Slice);
			std::string msg = StatusMessage::Format(_Slice, _MaxSlice);
			_StatusMapper->SetInput(msg.c_str());
			_ImageViewer->Render();
		}
	}


	virtual void OnKeyDown() {
		std::string key = this->GetInteractor()->GetKeySym();
		if (key.compare("Up") == 0) {
			//cout << "Up arrow key was pressed." << endl;
			MoveSliceForward();
		}
		else if (key.compare("Down") == 0) {
			//cout << "Down arrow key was pressed." << endl;
			MoveSliceBackward();
		}
		// forward event
		vtkInteractorStyleImage::OnKeyDown();
	}


	virtual void OnMouseWheelForward() {
		//std::cout << "Scrolled mouse wheel forward." << std::endl;
		MoveSliceForward();
		// don't forward events, otherwise the image will be zoomed 
		// in case another interactorstyle is used (e.g. trackballstyle, ...)
		// vtkInteractorStyleImage::OnMouseWheelForward();
	}


	virtual void OnMouseWheelBackward() {
		//std::cout << "Scrolled mouse wheel backward." << std::endl;
		if (_Slice > _MinSlice) {
			MoveSliceBackward();
		}
		// don't forward events, otherwise the image will be zoomed 
		// in case another interactorstyle is used (e.g. trackballstyle, ...)
		// vtkInteractorStyleImage::OnMouseWheelBackward();
	}
};

vtkStandardNewMacro(myVtkInteractorStyleImage);

MainWindow::MainWindow(QWidget *parent) :
QMainWindow(parent),
ui(new Ui::MainWindow)
{
	ui->setupUi(this);
	typedef itk::Image<signed short, 3> imagetype;
	typedef itk::ImageSeriesReader<imagetype> imagereadertype;
	typedef itk::ImageToVTKImageFilter<imagetype> connector;
	typedef itk::GDCMSeriesFileNames directorytype;
	typedef itk::GDCMImageIO imageiotype;
	typedef itk::GDCMSeriesFileNames InputNamesGeneratorType;

	imageiotype::Pointer dicom = imageiotype::New();
	InputNamesGeneratorType::Pointer inputNames = InputNamesGeneratorType::New();
	inputNames->SetInputDirectory("E:\\opengl\\dicom\\test1");
	imagereadertype::Pointer imagereader = imagereadertype::New();
	imagereader->SetImageIO(dicom);
	imagereader->SetFileNames(inputNames->GetInputFileNames());

	try{
		imagereader->Update();
	}
	catch (itk::ExceptionObject &excp){
		std::cerr << "Exception thrown while reading the series" << std::endl;
		std::cerr << excp << std::endl;
	}

	auto c1 = connector::New();
	c1->SetInput(imagereader->GetOutput());
	c1->Update();

	// Visualize
	vtkSmartPointer<vtkImageViewer2> imageViewer =
		vtkSmartPointer<vtkImageViewer2>::New();
	imageViewer->SetInputData(c1->GetOutput());

	// slice status message
	vtkSmartPointer<vtkTextProperty> sliceTextProp = vtkSmartPointer<vtkTextProperty>::New();
	sliceTextProp->SetFontFamilyToCourier();
	sliceTextProp->SetFontSize(20);
	sliceTextProp->SetVerticalJustificationToBottom();
	sliceTextProp->SetJustificationToLeft();

	vtkSmartPointer<vtkTextMapper> sliceTextMapper = vtkSmartPointer<vtkTextMapper>::New();
	std::string msg = StatusMessage::Format(imageViewer->GetSliceMin(), imageViewer->GetSliceMax());
	sliceTextMapper->SetInput(msg.c_str());
	sliceTextMapper->SetTextProperty(sliceTextProp);

	vtkSmartPointer<vtkActor2D> sliceTextActor = vtkSmartPointer<vtkActor2D>::New();
	sliceTextActor->SetMapper(sliceTextMapper);
	sliceTextActor->SetPosition(15, 10);

	// usage hint message
	vtkSmartPointer<vtkTextProperty> usageTextProp = vtkSmartPointer<vtkTextProperty>::New();
	usageTextProp->SetFontFamilyToCourier();
	usageTextProp->SetFontSize(14);
	usageTextProp->SetVerticalJustificationToTop();
	usageTextProp->SetJustificationToLeft();

	vtkSmartPointer<vtkTextMapper> usageTextMapper = vtkSmartPointer<vtkTextMapper>::New();
	usageTextMapper->SetInput("- Slice with mouse wheel\n  or Up/Down-Key\n- Zoom with pressed right\n  mouse button while dragging");
	usageTextMapper->SetTextProperty(usageTextProp);

	vtkSmartPointer<vtkActor2D> usageTextActor = vtkSmartPointer<vtkActor2D>::New();
	usageTextActor->SetMapper(usageTextMapper);
	usageTextActor->GetPositionCoordinate()->SetCoordinateSystemToNormalizedDisplay();
	usageTextActor->GetPositionCoordinate()->SetValue(0.05, 0.95);

	// create an interactor with our own style (inherit from vtkInteractorStyleImage)
	// in order to catch mousewheel and key events
	vtkSmartPointer<vtkRenderWindowInteractor> renderWindowInteractor =
		vtkSmartPointer<vtkRenderWindowInteractor>::New();

	vtkSmartPointer<myVtkInteractorStyleImage> myInteractorStyle =
		vtkSmartPointer<myVtkInteractorStyleImage>::New();

	// make imageviewer2 and sliceTextMapper visible to our interactorstyle
	// to enable slice status message updates when scrolling through the slices
	myInteractorStyle->SetImageViewer(imageViewer);
	myInteractorStyle->SetStatusMapper(sliceTextMapper);

	imageViewer->SetupInteractor(renderWindowInteractor);
	// make the interactor use our own interactorstyle
	// cause SetupInteractor() is defining it's own default interatorstyle 
	// this must be called after SetupInteractor()
	renderWindowInteractor->SetInteractorStyle(myInteractorStyle);
	// add slice status message and usage hint message to the renderer
	imageViewer->GetRenderer()->AddActor2D(sliceTextActor);
	imageViewer->GetRenderer()->AddActor2D(usageTextActor);
	imageViewer->SetSize(800, 600);

	// initialize rendering and interaction
	//imageViewer->GetRenderWindow()->SetSize(400, 300);
	//imageViewer->GetRenderer()->SetBackground(0.2, 0.3, 0.4);
	//imageViewer->Render();
	imageViewer->GetRenderer()->ResetCamera();
	//imageViewer->Render();
	//this->ui->qvtkWidget->GetRenderWindow()->AddRenderer(imageViewer->GetRenderer());
	renderWindowInteractor->Start();
}

void MainWindow::on_openBt1_clicked(){
	

	}

MainWindow::~MainWindow()
{
	delete ui;
}

std::string StatusMessage::Format(int _slice, int _maxSlice){
	std::stringstream _ss;
	_ss << "Slice Number" << _slice + 1 << "/" << _maxSlice + 1;
	return _ss.str();
}
