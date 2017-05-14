#include <gtest/gtest.h>
#include <opencv2/core.hpp>
#include <opencv2/imgproc.hpp>
#include <opencv2/highgui.hpp>

#if defined(OGLES_GPGPU_HAS_GLFW)
#  include <GLFW/glfw3.h>
#endif

// NOTE: GL_BGRA is absent in Android NDK
#ifdef ANDROID
#  define TEXTURE_FORMAT GL_RGBA
#else
#  define TEXTURE_FORMAT GL_BGRA
#endif

#include "../common/gl/memtransfer_optimized.h"

#include "../common/proc/yuv2rgb.h"      // [0]
#include "../common/proc/lnorm.h"        // [0]
#include "../common/proc/video.h"        // [0]
#include "../common/proc/adapt_thresh.h" // [x]
#include "../common/proc/gain.h"         // [x]
#include "../common/proc/blend.h"        // [x]
#include "../common/proc/grayscale.h"    // [x]
#include "../common/proc/fifo.h"         // [x]
#include "../common/proc/diff.h"         // [x]
#include "../common/proc/transform.h"    // [0]
#include "../common/proc/gauss.h"        // [0]
#include "../common/proc/gauss_opt.h"    // [0]
#include "../common/proc/box_opt.h"      // [0]
#include "../common/proc/hessian.h"      // [0]
#include "../common/proc/lbp.h"          // [0]
#include "../common/proc/median.h"       // [0]
#include "../common/proc/fir3.h"         // [0]
#include "../common/proc/grad.h"         // [0]
#include "../common/proc/iir.h"          // [?]
#include "../common/proc/lowpass.h"      // [0]
#include "../common/proc/highpass.h"     // [0]
#include "../common/proc/thresh.h"       // [0]
#include "../common/proc/pyramid.h"      // [0]
#include "../common/proc/ixyt.h"         // [0]
#include "../common/proc/tensor.h"       // [0]
#include "../common/proc/shitomasi.h"    // [0]
#include "../common/proc/harris.h"       // [0]
#include "../common/proc/nms.h"          // [0]
#include "../common/proc/flow.h"         // [0]
#include "../common/proc/rgb2hsv.h"      // [0]
#include "../common/proc/hsv2rgb.h"      // [0]
#include "../common/proc/remap.h"        // [ ] (needs work)

// virtual (tested indirectly)
//#include "../common/proc/filter3x3.h"
//#include "../common/proc/two.h"
//#include "../common/proc/three.h"

// NA:
//#include "../common/proc/disp.h"

int main(int argc, char **argv) {
    ::testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}

// Provide skeleton context when !defined(OGLES_GPGPU_HAS_GLFW)
// This supports end-to-end link tests.
struct GLFWContext {
    GLFWContext() {
#if defined(OGLES_GPGPU_HAS_GLFW)        
        // initialize glfw context
        glfwInit();
        glfwWindowHint(GLFW_VISIBLE, GLFW_FALSE);
        context = glfwCreateWindow(640, 480, "", NULL, NULL);
        glfwMakeContextCurrent(context);
        glActiveTexture(GL_TEXTURE0);
#endif
    }

    operator bool() const {
#if defined(OGLES_GPGPU_HAS_GLFW)                
        return (context != nullptr);
#else
        return false;
#endif
    }

    ~GLFWContext() {
#if defined(OGLES_GPGPU_HAS_GLFW)             
        glfwDestroyWindow(context);
        glfwTerminate();
#endif
    }

#if defined(OGLES_GPGPU_HAS_GLFW)     
    GLFWwindow *context = nullptr;
#endif
};


struct GLTexture {
    GLTexture(std::size_t width, std::size_t height, GLenum texType, void *data) {
        glGenTextures(1, &texId);
        glBindTexture(GL_TEXTURE_2D, texId);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
        glPixelStorei(GL_UNPACK_ALIGNMENT, 1);
        glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, width, height, 0, texType, GL_UNSIGNED_BYTE, data);
        glBindTexture(GL_TEXTURE_2D, 0);
    }

    ~GLTexture() {
        glDeleteTextures(1, &texId);
    }

    operator GLuint() const {
        return texId;
    }

    GLuint texId;
};

static cv::Mat getImage(ogles_gpgpu::ProcInterface &proc, cv::Mat &frame) {
    if(dynamic_cast<ogles_gpgpu::MemTransferOptimized*>(proc.getMemTransferObj())) {
        ogles_gpgpu::MemTransfer::FrameDelegate delegate = [&](const ogles_gpgpu::Size2d &size, const void *pixels, size_t bytesPerRow) {
            frame = cv::Mat(size.height, size.width, CV_8UC4, (void*)pixels, bytesPerRow).clone();
        };
        proc.getResultData(delegate);
    } else {
        frame.create(proc.getOutFrameH(), proc.getOutFrameW(), CV_8UC4); // noop if preallocated
        proc.getResultData(frame.ptr());
    }
    return frame;
}

static cv::Mat getTestImage(int width, int height, int stripe, bool alpha) {
    // Create a test image:
    cv::Mat test(480, 640, CV_8UC3, cv::Scalar::all(0));
    cv::Point center(test.cols/2, test.rows/2);
    for(int i = test.cols/2; i > 0; i -= stripe) {
        cv::circle(test, center, i, cv::Scalar(rand()%255, rand()%255, rand()%255), -1, 8);
    }

    if(alpha) {
        cv::cvtColor(test, test, cv::COLOR_BGR2BGRA); // add alpha
    }
    return test;
}

//######################
//### Shader Testing ###
//######################

TEST(OGLESGPGPUTest, Yuv2RgbProc) {
    GLFWContext context;
    if(context) {
        static const int width = 640, height = 480;

        cv::Mat green(1, 1, CV_8UC3, cv::Scalar(0,255,0)), yuv;
        cv::cvtColor(green, yuv, cv::COLOR_BGR2YUV);
        cv::Vec3b &value = yuv.at<cv::Vec3b>(0,0);

        // Create constant color buffers:
        std::vector<std::uint8_t> y(width*height, value[0]), uv(width*height/2);
        for(int i = 0; i < uv.size(); i += 2) {
            uv[i+0] = value[1];
            uv[i+1] = value[2];
        }

        // Luminance texture:
        GLTexture luminanceTexture(width, height, GL_LUMINANCE, y.data());
        ASSERT_EQ(glGetError(), GL_NO_ERROR);

        // Chrominance texture (interleaved):
        GLTexture chrominanceTexture(width/2, height/2, GL_LUMINANCE_ALPHA, uv.data());
        ASSERT_EQ(glGetError(), GL_NO_ERROR);

        ogles_gpgpu::Yuv2RgbProc yuv2rgb;
        yuv2rgb.init(width, height, 0, true);
        yuv2rgb.setExternalInputDataFormat(0); // for yuv
        yuv2rgb.getMemTransferObj()->setOutputPixelFormat(TEXTURE_FORMAT);
        yuv2rgb.createFBOTex(false);
        yuv2rgb.setTextures(luminanceTexture, chrominanceTexture);
        yuv2rgb.render();

        cv::Mat result;
        getImage(yuv2rgb, result);
        ASSERT_FALSE(result.empty());

        auto mu = cv::mean(result);
        //ASSERT_EQ(mu[0], 0);
        //ASSERT_EQ(mu[1], 255);
        //ASSERT_EQ(mu[2], 0);
    }
}

TEST(OGLESGPGPUTest, GrayScaleProc) {
    GLFWContext context;
    if(context) {
        cv::Mat test = getTestImage(640, 480, 10, true);
        glActiveTexture(GL_TEXTURE0);
        ogles_gpgpu::VideoSource video;
        ogles_gpgpu::GrayscaleProc gray;

        video.set(&gray);
        video({{test.cols, test.rows}, test.ptr<void>(), true, 0, TEXTURE_FORMAT});

        cv::Mat result;
        getImage(gray, result);
        ASSERT_FALSE(result.empty());
    }
}

TEST(OGLESGPGPUTest, AdaptThreshProc) {
    GLFWContext context;
    if(context) {
        cv::Mat test = getTestImage(640, 480, 10, true);
        glActiveTexture(GL_TEXTURE0);
        ogles_gpgpu::VideoSource video;
        ogles_gpgpu::AdaptThreshProc thresh;

        video.set(&thresh);
        video({{test.cols, test.rows}, test.ptr<void>(), true, 0, TEXTURE_FORMAT});

        cv::Mat result;
        getImage(thresh, result);
        ASSERT_FALSE(result.empty());
    }
}

TEST(OGLESGPGPUTest, GainProc) {
    GLFWContext context;
    if(context) {
        static const int value = 1, g = 10;
        cv::Mat test(640, 480, CV_8UC4, cv::Scalar(value,value,value,255));

        glActiveTexture(GL_TEXTURE0);
        ogles_gpgpu::VideoSource video;
        ogles_gpgpu::GainProc gain(g);

        video.set(&gain);
        video({{test.cols, test.rows}, test.ptr<void>(), true, 0, TEXTURE_FORMAT});

        cv::Mat result;
        getImage(gain, result);
        ASSERT_EQ(static_cast<int>(cv::mean(result)[0]), (value * g));
    }
}

TEST(OGLESGPGPUTest, BlendProc) {
    GLFWContext context;
    if(context) {
        const float alpha = 0.5f;
        const int value = 2;
        const int a = 1, b = 10;
        cv::Mat test(640, 480, CV_8UC4, cv::Scalar(value,value,value,255));

        glActiveTexture(GL_TEXTURE0);
        ogles_gpgpu::VideoSource video;
        ogles_gpgpu::GainProc gain1(a), gain10(b);
        ogles_gpgpu::BlendProc blend(alpha);

        gain1.add(&blend, 0);
        gain1.add(&gain10);
        gain10.add(&blend, 1);

        video.set(&gain1);
        video({test.cols, test.rows}, test.ptr<void>(), true, 0, TEXTURE_FORMAT);

        cv::Mat result;
        getImage(blend, result);
        ASSERT_EQ(static_cast<int>(cv::mean(result)[0]), static_cast<int>(static_cast<float>((value * a) + (value * a * b)) * alpha));
    }
}

TEST(OGLESGPGPUTest, FIFOProc) {
    GLFWContext context;
    if(context) {
        glActiveTexture(GL_TEXTURE0);
        ogles_gpgpu::VideoSource video;
        ogles_gpgpu::FIFOPRoc fifo(3);
        video.set(&fifo);

        for(int i = 0; i<3; i++) {
            cv::Mat test(640, 480, CV_8UC4, cv::Scalar(i,i,i,255));
            video({test.cols, test.rows}, test.ptr<void>(), true, 0, TEXTURE_FORMAT);
        }

        for(int i = 0; i < 3; i++) {
            cv::Mat result;
            getImage(*fifo[i], result);
            ASSERT_EQ(cv::mean(result)[0], i);
        }
    }
}

TEST(OGLESGPGPUTest, TransformProc) {
    GLFWContext context;
    if(context) {
        glActiveTexture(GL_TEXTURE0);
        ogles_gpgpu::VideoSource video;
        ogles_gpgpu::GainProc gain(1.f);
        ogles_gpgpu::TransformProc transform;

        video.set(&gain);
        gain.add(&transform);

        ogles_gpgpu::Mat44f matrix;
        for(int y = 0; y < 4; y++) {
            for(int x = 0; x < 4; x++) {
                matrix.data[y][x] = 0;
            }
            matrix.data[y][y] = 1.f;
        }
        matrix.data[0][0] = 0.25f;

        transform.setTransformMatrix(matrix);

        cv::Mat test = getTestImage(640, 480, 10, true);
        video({test.cols, test.rows}, test.ptr<void>(), true, 0, TEXTURE_FORMAT);

        cv::Mat result;
        getImage(transform, result);
        ASSERT_FALSE(result.empty());
    }
}

TEST(OGLESGPGPUTest, DiffProc) {
    GLFWContext context;
    if(context) {
        const int value = 2;
        const int a = 1, b = 10;
        cv::Mat test(640, 480, CV_8UC4, cv::Scalar(value,value,value,255));

        glActiveTexture(GL_TEXTURE0);
        ogles_gpgpu::VideoSource video;
        ogles_gpgpu::GainProc gain1(a), gain10(b);
        ogles_gpgpu::DiffProc diff;


        gain1.add(&diff, 1);
        gain1.add(&gain10);
        gain10.add(&diff, 0);

        video.set(&gain1);
        video({test.cols, test.rows}, test.ptr<void>(), true, 0, TEXTURE_FORMAT);

        cv::Mat result;
        getImage(diff, result);
        ASSERT_EQ(static_cast<int>(cv::mean(result)[0]), static_cast<int>(static_cast<float>((value * a * b) - (value * a))));
    }
}

TEST(OGLESGPGPUTest, GaussianProc) {
    GLFWContext context;
    if(context) {
        cv::Mat test = getTestImage(640, 480, 1, true);

        glActiveTexture(GL_TEXTURE0);
        ogles_gpgpu::VideoSource video;
        ogles_gpgpu::GaussProc gauss1, gauss2;

        video.set(&gauss1);
        gauss1.add(&gauss2);
        video({test.cols, test.rows}, test.ptr<void>(), true, 0, TEXTURE_FORMAT);

        cv::Mat result;
        getImage(gauss2, result);
        ASSERT_FALSE(result.empty());
    }
}

TEST(OGLESGPGPUTest, GaussianOptProc) {
    GLFWContext context;
    if(context) {
        cv::Mat test = getTestImage(640, 480, 1, true);

        glActiveTexture(GL_TEXTURE0);
        ogles_gpgpu::VideoSource video;
        ogles_gpgpu::GaussOptProc gauss;

        video.set(&gauss);
        video({test.cols, test.rows}, test.ptr<void>(), true, 0, TEXTURE_FORMAT);

        cv::Mat result;
        getImage(gauss, result);
        ASSERT_FALSE(result.empty());
    }
}

TEST(OGLESGPGPUTest, BoxOptProc) {
    GLFWContext context;
    if(context) {
        cv::Mat test = getTestImage(640, 480, 1, true);

        glActiveTexture(GL_TEXTURE0);
        ogles_gpgpu::VideoSource video;
        ogles_gpgpu::BoxOptProc box;

        video.set(&box);
        video({test.cols, test.rows}, test.ptr<void>(), true, 0, TEXTURE_FORMAT);

        cv::Mat result;
        getImage(box, result);
        ASSERT_FALSE(result.empty());
    }
}

TEST(OGLESGPGPUTest, HessianProc) {
    GLFWContext context;
    if(context) {
        cv::Mat test(480, 640, CV_8UC1, cv::Scalar::all(0));

        static const int tic = 20;
        for(int y = tic; y <= test.rows-tic; y+=tic) {
            for(int x = tic; x <= test.cols-tic; x+=tic) {
                cv::circle(test, {x,y}, 1, 255, -1, 8);
            }
        }
        cv::GaussianBlur(test, test, {7,7}, 1.0);
        cv::cvtColor(test, test, cv::COLOR_GRAY2BGR);
        cv::cvtColor(test, test, cv::COLOR_BGR2BGRA);

        glActiveTexture(GL_TEXTURE0);
        ogles_gpgpu::VideoSource video;
        ogles_gpgpu::HessianProc hessian(100.f);

        video.set(&hessian);
        video({test.cols, test.rows}, test.ptr<void>(), true, 0, TEXTURE_FORMAT);

        cv::Mat result, alpha;
        getImage(hessian, result);
        cv::extractChannel(result, alpha, 3);
        ASSERT_FALSE(alpha.empty());
    }
}

TEST(OGLESGPGPUTest, LbpProc) {
    GLFWContext context;
    if(context) {
        cv::Mat test = getTestImage(480, 640, 1, true);

        glActiveTexture(GL_TEXTURE0);
        ogles_gpgpu::VideoSource video;
        ogles_gpgpu::LbpProc lbp;

        video.set(&lbp);
        video({test.cols, test.rows}, test.ptr<void>(), true, 0, TEXTURE_FORMAT);

        cv::Mat result;
        getImage(lbp, result);
        ASSERT_FALSE(result.empty());
    }
}

TEST(OGLESGPGPUTest, MedianProc) {
    GLFWContext context;
    if(context) {
        cv::Mat test = getTestImage(480, 640, 20, true);

        glActiveTexture(GL_TEXTURE0);
        ogles_gpgpu::VideoSource video;
        ogles_gpgpu::MedianProc median;

        video.set(&median);

        cv::Mat noise = cv::Mat::zeros(test.rows, test.cols, CV_8UC1);
        cv::randu(noise,0,255);
        test.setTo(0, noise < 30);
        test.setTo(255, noise > 225);

        video({test.cols, test.rows}, test.ptr<void>(), true, 0, TEXTURE_FORMAT);

        cv::Mat result;
        getImage(median, result);
        ASSERT_FALSE(result.empty());
    }
}

TEST(OGLESGPGPUTest, Fir3Proc) {
    GLFWContext context;
    if(context) {
        const cv::Size size(640, 480);
        const cv::Point center(size.width/2, size.height/2);
        const float radius = size.height/2;

        std::vector<cv::Mat> test(3);
        std::vector<int> values { 64, 128, 64 };

        for(int i = 0; i < 3; i++) {
            auto &value = values[i];
            cv::Mat canvas(size, CV_8UC3, cv::Scalar(value, value, value, 255));
            cv::circle(canvas, center, radius, values[i], -1, 4);
            cv::cvtColor(canvas, canvas, cv::COLOR_BGR2BGRA);
            test[i] = canvas;
        }

        static const bool doRgb = true;

        glActiveTexture(GL_TEXTURE0);
        ogles_gpgpu::VideoSource video;
        ogles_gpgpu::GainProc gain(1.f); // noop
        ogles_gpgpu::FifoProc fifo(3);
        ogles_gpgpu::Fir3Proc fir3(doRgb);
        fir3.setWeights( {-0.25f, -0.25f, -0.25f}, {+0.50f, +0.50f, +0.50f}, {-0.25f, -0.25f, -0.25f} );

        video.set(&gain);
        gain.add(&fifo);
        fifo.addWithDelay(&fir3, 0, 0);
        fifo.addWithDelay(&fir3, 1, 1);
        fifo.addWithDelay(&fir3, 2, 2);

        for(int i = 0; i < 3; i++) {
            video({test[i].cols, test[i].rows}, test[i].ptr<void>(), true, 0, TEXTURE_FORMAT);
        }

        cv::Mat result;
        getImage(fir3, result);
        ASSERT_FALSE(result.empty());
    }
}

TEST(OGLESGPGPUTest, GradProc) {
    GLFWContext context;
    if(context) {
        cv::Mat test = getTestImage(640, 480, 2, true);

        glActiveTexture(GL_TEXTURE0);
        ogles_gpgpu::VideoSource video;
        ogles_gpgpu::GradProc grad;

        video.set(&grad);
        video({test.cols, test.rows}, test.ptr<void>(), true, 0, TEXTURE_FORMAT);

        cv::Mat result;
        getImage(grad, result);
        ASSERT_FALSE(result.empty());
    }
}


TEST(OGLESGPGPUTest, LowPassProc) {
    GLFWContext context;
    if(context) {
        std::vector<cv::Mat> images {
            cv::Mat(480, 640, CV_8UC4, cv::Scalar(0,0,0,255)),
            cv::Mat(480, 640, CV_8UC4, cv::Scalar(255,255,255,255))
        };

        glActiveTexture(GL_TEXTURE0);
        ogles_gpgpu::VideoSource video;
        ogles_gpgpu::LowPassFilterProc low;
        video.set(&low);

        for(int i = 0; i < 5; i++) {
            cv::Mat &test = images[i%2];
            video({test.cols, test.rows}, test.ptr<void>(), true, 0, TEXTURE_FORMAT);
        }

        cv::Mat result;
        getImage(low, result);
        ASSERT_FALSE(result.empty());
    }
}

TEST(OGLESGPGPUTest, HighPassProc) {
    GLFWContext context;
    if(context) {
        std::vector<cv::Mat> images {
            cv::Mat(480, 640, CV_8UC4, cv::Scalar(0,0,0,255)),
            cv::Mat(480, 640, CV_8UC4, cv::Scalar(255,255,255,255))
        };

        glActiveTexture(GL_TEXTURE0);
        ogles_gpgpu::VideoSource video;
        ogles_gpgpu::HighPassFilterProc high;
        video.set(&high);

        for(int i = 0; i < 5; i++) {
            cv::Mat &test = images[i%2];
            video({test.cols, test.rows}, test.ptr<void>(), true, 0, TEXTURE_FORMAT);
        }

        cv::Mat result;
        getImage(high, result);
        ASSERT_FALSE(result.empty());
    }
}

TEST(OGLESGPGPUTest, ThreshProc) {
    GLFWContext context;
    if(context) {
        cv::Mat test = getTestImage(640, 480, 2, true);

        glActiveTexture(GL_TEXTURE0);
        ogles_gpgpu::VideoSource video;
        ogles_gpgpu::ThreshProc thresh;

        video.set(&thresh);
        video({test.cols, test.rows}, test.ptr<void>(), true, 0, TEXTURE_FORMAT);

        cv::Mat result;
        getImage(thresh, result);
        ASSERT_FALSE(result.empty());
    }
}

TEST(OGLESGPGPUTest, PyramidProc) {
    GLFWContext context;
    if(context) {
        cv::Mat test = getTestImage(640, 480, 2, true);

        glActiveTexture(GL_TEXTURE0);
        ogles_gpgpu::VideoSource video;
        ogles_gpgpu::PyramidProc pyramid(10);
        video.set(&pyramid);

        video({test.cols, test.rows}, test.ptr<void>(), true, 0, TEXTURE_FORMAT);

        cv::Mat result;
        getImage(pyramid, result);
        ASSERT_FALSE(result.empty());
    }
}

TEST(OGLESGPGPUTest, IxytProc) {
    GLFWContext context;

    if(context) {
        glActiveTexture(GL_TEXTURE0);
        ogles_gpgpu::VideoSource video;
        ogles_gpgpu::GainProc gain(1.f);
        ogles_gpgpu::FifoProc fifo(2);
        ogles_gpgpu::IxytProc ixyt;
        ixyt.setWaitForSecondTexture(true);

        video.set(&gain);
        gain.add(&fifo);
        fifo.addWithDelay(&ixyt, 1, 1);
        fifo.addWithDelay(&ixyt, 0, 0);

        for(int i = 0; i < 5; i++) {
            cv::Mat test = getTestImage(640, 480, 2, true);
            video({test.cols, test.rows}, test.ptr<void>(), true, 0, TEXTURE_FORMAT);
        }

        cv::Mat result;
        getImage(ixyt, result);

        std::vector<cv::Mat> channels;
        cv::split(result, channels);
        cv::hconcat(channels, result);
        ASSERT_FALSE(result.empty());
    }
}

TEST(OGLESGPGPUTest, TensorProc) {
    GLFWContext context;
    if(context) {
        cv::Mat test = getTestImage(640, 480, 2, true);

        glActiveTexture(GL_TEXTURE0);
        ogles_gpgpu::VideoSource video;
        ogles_gpgpu::TensorProc tensor;

        video.set(&tensor);
        video({test.cols, test.rows}, test.ptr<void>(), true, 0, TEXTURE_FORMAT);

        cv::Mat result;
        getImage(tensor, result);
        ASSERT_FALSE(result.empty());
    }
}

TEST(OGLESGPGPUTest, ShiTomasiProc) {
    GLFWContext context;
    if(context) {
        cv::Mat test = getTestImage(640, 480, 2, true);
        for(int i = 0; i < 100; i++) {
            cv::Point p0(rand()%test.cols, rand()%test.rows);
            cv::Point p1(rand()%test.cols, rand()%test.rows);
            cv::line(test, p0, p1, {0,255,0}, 2, 8);
        }

        glActiveTexture(GL_TEXTURE0);
        ogles_gpgpu::VideoSource video;
        ogles_gpgpu::GaussOptProc gauss;
        ogles_gpgpu::TensorProc tensor;
        ogles_gpgpu::ShiTomasiProc shiTomasi;

        tensor.setEdgeStrength(100.f);
        shiTomasi.setSensitivity(100000000.f);

        video.set(&gauss);
        gauss.add(&tensor);
        tensor.add(&shiTomasi);

        video({test.cols, test.rows}, test.ptr<void>(), true, 0, TEXTURE_FORMAT);

        cv::Mat result;
        getImage(shiTomasi, result);
        ASSERT_FALSE(result.empty());
    }
}

TEST(OGLESGPGPUTest, HarrisProc) {
    GLFWContext context;
    if(context) {
        cv::Mat test = getTestImage(640, 480, 2, true);
        for(int i = 0; i < 100; i++) {
            cv::Point p0(rand()%test.cols, rand()%test.rows);
            cv::Point p1(rand()%test.cols, rand()%test.rows);
            cv::line(test, p0, p1, {0,255,0}, 2, 8);
        }

        glActiveTexture(GL_TEXTURE0);
        ogles_gpgpu::VideoSource video;
        ogles_gpgpu::GaussOptProc gauss;
        ogles_gpgpu::TensorProc tensor;
        ogles_gpgpu::HarrisProc harris;

        tensor.setEdgeStrength(100.f);
        harris.setSensitivity(100000000.f);

        video.set(&gauss);
        gauss.add(&tensor);
        tensor.add(&harris);

        video({test.cols, test.rows}, test.ptr<void>(), true, 0, TEXTURE_FORMAT);

        cv::Mat result;
        getImage(harris, result);
        ASSERT_FALSE(result.empty());
    }
}

TEST(OGLESGPGPUTest, NmsProc) {
    GLFWContext context;
    if(context) {
        cv::Mat test(480, 640, CV_8UC1, cv::Scalar::all(0));

        static const int tic = 20;
        for(int y = tic; y <= test.rows-tic; y+=tic) {
            for(int x = tic; x <= test.cols-tic; x+=tic) {
                cv::circle(test, {x,y}, 1, 255, -1, 8);
            }
        }
        cv::GaussianBlur(test, test, {7,7}, 1.0);
        cv::cvtColor(test, test, cv::COLOR_GRAY2BGR);
        cv::cvtColor(test, test, cv::COLOR_BGR2BGRA);

        glActiveTexture(GL_TEXTURE0);
        ogles_gpgpu::VideoSource video;
        ogles_gpgpu::HessianProc hessian(100.f);
        ogles_gpgpu::NmsProc nms;

        video.set(&hessian);
        hessian.add(&nms);
        video({test.cols, test.rows}, test.ptr<void>(), true, 0, TEXTURE_FORMAT);

        cv::Mat result, alpha;
        getImage(nms, result);
        cv::extractChannel(result, result, 3);
        ASSERT_FALSE(result.empty());
    }
}

TEST(OGLESGPGPUTest, FlowProc) {
    GLFWContext context;
    if(context) {
        cv::Mat test = getTestImage(480, 640, 3, true);

        glActiveTexture(GL_TEXTURE0);
        ogles_gpgpu::VideoSource video;
        ogles_gpgpu::GainProc gain(1.f);
        ogles_gpgpu::Flow2Pipeline flow;

        video.set(&gain);
        gain.add(&flow);

        for(int i = 0; i < 5; i++) {
            cv::Mat shifted;
            cv::Matx23f M(1,0,i*4,0,1,i*4);
            cv::warpAffine(test, shifted, M, test.size());
            video({shifted.cols, shifted.rows}, shifted.ptr<void>(), true, 0, TEXTURE_FORMAT);
        }

        cv::Mat result;
        getImage(flow, result);
        ASSERT_FALSE(result.empty());
    }
}

TEST(OGLESGPGPUTest, Rgb2HsvProc) {
    GLFWContext context;
    if(context) {
        cv::Mat test = getTestImage(480, 640, 3, true);

        glActiveTexture(GL_TEXTURE0);
        ogles_gpgpu::VideoSource video;
        ogles_gpgpu::Rgb2HsvProc rgb2hsv;

        video.set(&rgb2hsv);
        video({test.cols, test.rows}, test.ptr<void>(), true, 0, TEXTURE_FORMAT);

        cv::Mat result;
        getImage(rgb2hsv, result);
        ASSERT_FALSE(result.empty());
    }
}

TEST(OGLESGPGPUTest, Hsv2RgbProc) {
    GLFWContext context;
    if(context) {
        cv::Mat test = getTestImage(480, 640, 3, true);

        glActiveTexture(GL_TEXTURE0);
        ogles_gpgpu::VideoSource video;
        ogles_gpgpu::Hsv2RgbProc hsv2rgb;

        video.set(&hsv2rgb);
        video({test.cols, test.rows}, test.ptr<void>(), true, 0, TEXTURE_FORMAT);

        cv::Mat result;
        getImage(hsv2rgb, result);
        ASSERT_FALSE(result.empty());
    }
}

TEST(OGLESGPGPUTest, LNormProc) {
    GLFWContext context;
    if(context) {
        cv::Mat test = getTestImage(480, 640, 3, true);

        glActiveTexture(GL_TEXTURE0);
        ogles_gpgpu::VideoSource video;
        ogles_gpgpu::GainProc gainProc(1.f);
        ogles_gpgpu::LocalNormProc normProc;

        video.set(&gainProc);
        gainProc.add(&normProc);
        video({test.cols, test.rows}, test.ptr<void>(), true, 0, TEXTURE_FORMAT);

        cv::Mat result;
        getImage(normProc, result);
        ASSERT_FALSE(result.empty());
    }
}
