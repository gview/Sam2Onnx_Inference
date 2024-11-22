#include <iostream>
#include <filesystem>
#include "SAM2.h"
#include <onnxruntime_cxx_api.h>

void sam2() {
    std::vector<std::string> onnx_paths{
        "D:/sam2_c++/Project1/model/model_sam2/tiny_encoder.onnx",
        "D:/sam2_c++/Project1/model/model_sam2/tiny_decoder.onnx",
    };
    auto sam2 = std::make_unique<SAM2>();
    auto r = sam2->initialize(onnx_paths,false);
    if (r.index() != 0) {
        std::string error = std::get<std::string>(r);
        std::cout << ("����{}", error);
        return;
    }

    int type = 0;
    cv::Rect prompt_box = { 1087,1200,1000,1000 };//xywh
    cv::Point prompt_point = { 1373, 1682 };
    sam2->setparms(type, prompt_box, prompt_point); // ��ԭʼͼ���ϵ�box,point
    std::string video_path = "E:/LYX_date/SAM_data/SAM_data/sam2_lianxu/2.mp4";
    cv::VideoCapture capture(video_path);
    if (!capture.isOpened()) return;
    //************************************************************
    std::cout << "��Ƶ��ͼ��Ŀ��=" << capture.get(cv::CAP_PROP_FRAME_WIDTH) << std::endl;
    std::cout << "��Ƶ��ͼ��ĸ߶�=" << capture.get(cv::CAP_PROP_FRAME_HEIGHT) << std::endl;
    std::cout << "��Ƶ֡��=" << capture.get(cv::CAP_PROP_FPS) << std::endl;
    std::cout << "��Ƶ����֡��=" << capture.get(cv::CAP_PROP_FRAME_COUNT) << std::endl;
    //************************************************************
    cv::Mat frame;
    size_t idx = 0;
    std::string window_name = "frame";
    cv::namedWindow(window_name, cv::WINDOW_NORMAL);
    cv::resizeWindow(window_name, 1980,1080 ); // ���罫���ڴ�С����Ϊ 640x480
    while (true) {
        frame = cv::imread("E:/LYX_date/mapImage/mapImage/00003595_999613_1107_1206_970_968_0.980981.jpg");
        auto start = std::chrono::high_resolution_clock::now();
        auto result = sam2->inference(frame);
        auto end = std::chrono::high_resolution_clock::now();
        auto duration = std::chrono::duration_cast<std::chrono::milliseconds>(end - start).count();
        std::cout << ("frame = {},duration = {}ms", idx++, duration) << std::endl;
        if (result.index() == 0) {
            //std::string text = std::format("frame = {},fps={:.1f}", idx, 1000.0f / duration);
            std::string text = "frame = " + std::to_string(idx) + ", fps = " + std::to_string(1000.0f / duration);
            cv::putText(frame, text, cv::Point{ 30,40 }, 1, 2, cv::Scalar(0, 0, 255), 2);
            cv::imshow(window_name, frame);
            int key = cv::waitKey(1000);
            if (key == 'q' || key == 27) break;
        }
        else {
            std::string error = std::get<std::string>(result);
            std::cout << ("����{}", error);
            break;
        }
    }
    capture.release();
}

bool isGpuAvailable() {
    try {
        // ����һ�� ONNX Runtime ����
        Ort::Env env(ORT_LOGGING_LEVEL_WARNING, "ONNXRuntime");

        // ʹ�� GPU��CUDA���ṩ������ѡ��
        Ort::SessionOptions session_options;
        session_options.SetGraphOptimizationLevel(GraphOptimizationLevel::ORT_ENABLE_ALL);

        // ���Խ�ִ���ṩ������Ϊ CUDA
#if defined(ORT_CUDA)
        session_options.AppendExecutionProvider_CUDA(0); // ʹ�õ�0�� GPU
#else
        std::cerr << "CUDA is not enabled in this build of ONNX Runtime." << std::endl;
        return false;
#endif

        // ����һ���յ� session�����ڲ��� CUDA �ṩ���Ƿ���������
        // ����ģ���ļ� "dummy_model.onnx" ���ڲ�����һ��С��ģ�ͣ����ڲ��� GPU ������
        const ORTCHAR_T* modelPath = L"D:/sam2/SAM2Export-main/image_encoder.onnx";
        Ort::Session test_session(env, modelPath, session_options);

        // ����ܹ��ɹ����� session����˵�� GPU ����
        return true;
    }
    catch (const Ort::Exception& e) {
        std::cerr << "GPU is not available: " << e.what() << std::endl;
        return false;
    }
}

int main(int argc, char const* argv[]) {

    sam2();
    return 0;
}




