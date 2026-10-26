/*
 * TrueGlide - carte simulée : liaison avec l'extérieur.
 * Free software under the GPL-3.0 license, WITHOUT ANY WARRANTY; see the LICENSE file.
 *
 * Navigateur (WebAssembly) : la page envoie les octets avec tg_entree() et les reçoit par Module.tgSortie(port,
 * octets) ; un redémarrage appelle Module.tgRedemarrage() après l'enregistrement de eeprom.bin.
 * Ordinateur (essais) : le port USB est un serveur TCP sur le port 5761, comme le simulateur SITL.
 */

#include <stdbool.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "platform.h"

#include "common/utils.h"

#include "simulee/liaison.h"

#ifdef __EMSCRIPTEN__
#include <emscripten.h>
#else
#include <errno.h>
#include <fcntl.h>
#include <netinet/in.h>
#include <netinet/tcp.h>
#include <sys/socket.h>
#include <unistd.h>
#endif

#define TG_PORTS 1
#define TG_TAMPON 4096

typedef struct {
    uint8_t octets[TG_TAMPON];
    uint32_t tete;
    uint32_t queue;
} tampon_t;

static tampon_t entrees[TG_PORTS];

void tg_entree(int port, uint8_t *octets, int nombre)
{
    if (port < 0 || port >= TG_PORTS) {
        return;
    }
    tampon_t *t = &entrees[port];
    for (int i = 0; i < nombre; i++) {
        const uint32_t suivante = (t->tete + 1) % TG_TAMPON;
        if (suivante == t->queue) {
            break; // plein : comme un vrai port, les octets en trop sont perdus
        }
        t->octets[t->tete] = octets[i];
        t->tete = suivante;
    }
}

uint32_t tgEnAttente(int port)
{
    tgRelever();
    const tampon_t *t = &entrees[port];
    return (t->tete + TG_TAMPON - t->queue) % TG_TAMPON;
}

uint8_t tgLire(int port)
{
    tampon_t *t = &entrees[port];
    if (t->tete == t->queue) {
        return 0;
    }
    const uint8_t c = t->octets[t->queue];
    t->queue = (t->queue + 1) % TG_TAMPON;
    return c;
}

#ifdef __EMSCRIPTEN__

EM_JS(void, tgSortieJs, (int port, const uint8_t *octets, int nombre), {
    if (Module.tgSortie) Module.tgSortie(port, HEAPU8.slice(octets, octets + nombre));
});

EM_JS(void, tgRedemarrageJs, (void), {
    if (Module.tgRedemarrage) Module.tgRedemarrage();
});

void tgEcrire(int port, const uint8_t *octets, int nombre)
{
    tgSortieJs(port, octets, nombre);
}

void tgRelever(void)
{
}

void tgRedemarrer(void)
{
    emscripten_cancel_main_loop();
    tgRedemarrageJs();
    exit(0);
}

#else

static int serveur = -1;
static int client = -1;

static void tcpOuvrir(void)
{
    serveur = socket(AF_INET, SOCK_STREAM, 0);
    const int oui = 1;
    setsockopt(serveur, SOL_SOCKET, SO_REUSEADDR, &oui, sizeof(oui));
    struct sockaddr_in a = { .sin_family = AF_INET, .sin_port = htons(5761), .sin_addr.s_addr = htonl(INADDR_LOOPBACK) };
    if (bind(serveur, (struct sockaddr *)&a, sizeof(a)) || listen(serveur, 1)) {
        fprintf(stderr, "[carte] port TCP 5761 indisponible\n");
        exit(1);
    }
    fcntl(serveur, F_SETFL, O_NONBLOCK);
    fprintf(stderr, "[carte] port USB sur TCP 5761\n");
}

void tgRelever(void)
{
    if (serveur < 0) {
        tcpOuvrir();
    }
    if (client < 0) {
        client = accept(serveur, NULL, NULL);
        if (client >= 0) {
            fcntl(client, F_SETFL, O_NONBLOCK);
            const int oui = 1;
            setsockopt(client, IPPROTO_TCP, TCP_NODELAY, &oui, sizeof(oui));
        }
        return;
    }
    uint8_t lu[512];
    const ssize_t n = recv(client, lu, sizeof(lu), 0);
    if (n > 0) {
        tg_entree(TG_PORT_USB, lu, (int)n);
    } else if (n == 0 || (errno != EAGAIN && errno != EWOULDBLOCK)) {
        close(client);
        client = -1;
    }
}

void tgEcrire(int port, const uint8_t *octets, int nombre)
{
    if (port == TG_PORT_USB && client >= 0) {
        while (nombre > 0) {
            const ssize_t n = send(client, octets, nombre, MSG_NOSIGNAL);
            if (n <= 0) {
                if (n < 0 && (errno == EAGAIN || errno == EWOULDBLOCK)) {
                    continue;
                }
                break;
            }
            octets += n;
            nombre -= (int)n;
        }
    }
}

void tgRedemarrer(void)
{
    // la boucle d'essai relance le programme, qui relit eeprom.bin
    if (client >= 0) {
        close(client);
    }
    exit(0);
}

#endif

// identifiant unique de la puce simulée : la page peut donner celui de la carte récoltée, pour que l'envoi des
// réglages préparés reconnaisse la même carte
#ifdef __EMSCRIPTEN__
EMSCRIPTEN_KEEPALIVE
#endif
void tg_definir_uid(uint32_t uid0, uint32_t uid1, uint32_t uid2)
{
    tgIdentifiantUnique[0] = uid0;
    tgIdentifiantUnique[1] = uid1;
    tgIdentifiantUnique[2] = uid2;
}
