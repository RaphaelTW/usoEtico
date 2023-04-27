#include <iostream>
#include <fstream>
#include <GeoIP.h>
#include <opencv2/opencv.hpp>
#include <opencv2/highgui.hpp>
#include <opencv2/imgproc.hpp>

using namespace std;
using namespace cv;

int main() {
    // Acessando a localização do dispositivo
    const char* dbfile = "/path/to/GeoIP.dat"; // Substitua pelo caminho do arquivo GeoIP no seu sistema
    GeoIP* gi = GeoIP_open(dbfile, GEOIP_STANDARD);
    GeoIPRecord* gir = GeoIP_record_by_addr(gi, "IP_DO_DISPOSITIVO"); // Substitua "IP_DO_DISPOSITIVO" pelo endereço IP do dispositivo que você deseja monitorar
    double latitude = gir->latitude;
    double longitude = gir->longitude;
    GeoIPRecord_delete(gir);
    GeoIP_delete(gi);

    // Tirando uma foto usando a câmera frontal do dispositivo
    VideoCapture cap(1); // Seleciona a câmera frontal com índice 1
    cap.set(CAP_PROP_FOURCC, CV_FOURCC('M', 'J', 'P', 'G')); // Define o formato de vídeo para MJPG
    Mat frame;
    cap >> frame; // Captura o frame atual da câmera
    
    // Salvando a foto em um arquivo
    string filename = "foto.jpg";
    imwrite(filename, frame); // Salva o frame em um arquivo

    // Escrevendo a localização e o nome do arquivo em um arquivo de texto
    ofstream myfile;
    myfile.open("localizacao.txt");
    myfile << "Latitude: " << latitude << "\n";
    myfile << "Longitude: " << longitude << "\n";
    myfile << "Foto: " << filename << "\n";
    myfile.close();
    
    // Exibindo a foto
    namedWindow("Foto", WINDOW_NORMAL);
    imshow("Foto", frame);
    waitKey(0);

    return 0;
}
