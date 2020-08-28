#include <SFML/Network.hpp>
#include <iostream>
#include <fstream>
#include <vector>

using namespace sf;
using namespace std;

int PORT = 30780;


int main() {

    vector<TcpSocket*> clientSockets;
    TcpListener listener;


    if (listener.listen(PORT) != Socket::Status::Done) {
        cout << "Cannot listen on port " << PORT << endl;

        return 0;
    }

    cout << "Server listening at " << PORT << endl;

    listener.setBlocking(false);

    while (true) {
        TcpSocket* client = new TcpSocket;
        client->setBlocking(false);

        if (listener.accept(*client) == Socket::Status::Done) {
            cout << "Client " << client->getRemoteAddress() << " connected" << endl;

            fstream scores;
            scores.open("scores.txt", ios::in);
            string top1name, top1score;
            string top2name, top2score;
            string top3name, top3score;
            scores >> top1name >> top1score;
            scores >> top2name >> top2score;
            scores >> top3name >> top3score;

            scores.close();

            Packet packet;
            packet << top1name + ":" + top1score;
            packet << top2name + ":" + top2score;
            packet << top3name + ":" + top3score;
            client->send(packet);

            clientSockets.push_back(client);
        } else {
            delete client;
        }

        for (int i = 0; i < clientSockets.size(); i++) {
            Packet packet;
            TcpSocket* currentClientSocket = clientSockets.at(i);
            Socket::Status status = currentClientSocket->receive(packet);

            if (status == Socket::Status::Disconnected) {
                clientSockets.erase(clientSockets.begin() + i);

                cout << "Client " << currentClientSocket->getRemoteAddress() << " disconnected" << endl;
            } else if (status == Socket::Done) {
                string type;

                packet >> type;

                if (type == "game over") {
                    string name, score;
                    packet >> name >> score;

                    cout << name << ":" << score << endl;

                    fstream scores;
                    scores.open("scores.txt", ios::in);
                    string top1name, top1score;
                    string top2name, top2score;
                    string top3name, top3score;
                    scores >> top1name >> top1score;
                    scores >> top2name >> top2score;
                    scores >> top3name >> top3score;

                    scores.close();

                    if (stoi(score) > stoi(top1score)) {
                        top1name = name;
                        top1score = score;
                    } else if (stoi(score) > stoi(top2score)) {
                        top2name = name;
                        top2score = score;
                    } else if (stoi(score) > stoi(top3score)) {
                        top3name = name;
                        top3score = score;
                    }

                    scores.open("scores.txt", ios::out | ios::trunc);

                    scores << top1name << " " << top1score << endl;
                    scores << top2name << " " << top2score << endl;
                    scores << top3name << " " << top3score << endl;

                    scores.close();
                } else if (type == "get scores") {
                    cout << type << endl;

                    fstream scores;
                    scores.open("scores.txt", ios::in);
                    string top1name, top1score;
                    string top2name, top2score;
                    string top3name, top3score;
                    scores >> top1name >> top1score;
                    scores >> top2name >> top2score;
                    scores >> top3name >> top3score;

                    scores.close();

                    Packet packet;
                    packet << top1name + ":" + top1score;
                    packet << top2name + ":" + top2score;
                    packet << top3name + ":" + top3score;
                    currentClientSocket->send(packet);

                }
            }
        }
    }

    return 0;
}