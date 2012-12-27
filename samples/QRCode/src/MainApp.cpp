#include "cinder/app/AppBasic.h"

#include "cinder/gl/gl.h"
#include "cinder/gl/Texture.h"
#include "cinder/Surface.h"
#include "cinder/Capture.h"


#include <zxing/qrcode/QRCodeReader.h>
#include <zxing/common/GlobalHistogramBinarizer.h>
#include <zxing/Exception.h>
#include <zxing/DecodeHints.h>

#include "CinderZXing.h"

using namespace ci;
using namespace ci::app;
using namespace std;

class MainApp : public AppBasic {
 public:
	void setup();
    void mouseDown(MouseEvent event);
    void update();
	void draw();
    
    Capture     mCapture;
    Surface8u   mCaptureImg;
    gl::Texture mCaptureTex;
    bool        mDetected;
    string      mData;
    
};

void MainApp::setup()
{
    
    setWindowSize(640, 480);
    
    mDetected = false;
    
    try {
		mCapture = Capture( 640, 480 );
		mCapture.start();
	}
	catch( ... ) {
		console() << "Failed to initialize capture" << std::endl;
	}
    
}


void MainApp::mouseDown(MouseEvent event)
{
    
}

void MainApp::update()
{
    
    if( mCapture && mCapture.checkNewFrame() ) {
        mCaptureImg = mCapture.getSurface();
		mCaptureTex = gl::Texture( mCaptureImg );
        
        mDetected = false;
        
        try {
            zxing::Ref<zxing::SurfaceBitmapSource> source(new zxing::SurfaceBitmapSource(mCaptureImg));
            
            zxing::Ref<zxing::Binarizer> binarizer(NULL);
            binarizer = new zxing::GlobalHistogramBinarizer(source);
            
            zxing::Ref<zxing::BinaryBitmap> image(new zxing::BinaryBitmap(binarizer));
            zxing::qrcode::QRCodeReader reader;
            zxing::DecodeHints hints(zxing::DecodeHints::BARCODEFORMAT_QR_CODE_HINT);
            
            zxing::Ref<zxing::Result> result( reader.decode(image, hints) );
            
            console() << "READ(" << result->count() << ") : " << result->getText()->getText() << endl;
            
            if( result->count() ) {
                mDetected = true;
                mData = result->getText()->getText();
            }
            
        } catch (zxing::Exception& e) {
            cerr << "Error: " << e.what() << endl;
        }
        
	}
}

void MainApp::draw() 
{
    gl::clear( Color( 0.1f, 0.1f, 0.1f ) );

    gl::color(Color::white());

    if(mCaptureTex) {
        gl::draw(mCaptureTex);
        
    }
    
    if(mDetected) {
        Vec2f pos = Vec2f( getWindowWidth()*0.5f, getWindowHeight()-100.f );
        gl::drawStringCentered(mData, pos);
    }
    
}

CINDER_APP_BASIC( MainApp, RendererGl )
