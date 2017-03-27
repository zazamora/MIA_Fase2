#ifndef USERGROUP_H
#define USERGROUP_H

#include "[MIA]edd_201313952.h"
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <time.h>
#include <math.h>

char *listaU[8];
char *subListaU[5];
char *usuario;
char *password;
int logueado = 0;
int ncont = 0;
int mcont = 0;
int auxRmv = -1;
char auxD[25];

char *buscarRutaArchivo(char *idP){
    /**************** SE UBICA LA RUTA DEL DISCO DE LA PARTICION *******************/
    char *ruta;
    ruta = malloc(100);
    disco tempDisc;
    FILE *indx = fopen("/home/saul/Desktop/manejador.dsk", "rb+");
    if (indx) {
        fseek(indx, 0, SEEK_SET);
        while (fread(&tempDisc, sizeof (disco), 1, indx)) {
            if (strncmp(tempDisc.llave, idP, 2) == 0) {
                printf("<*> Llave Disco = %s \n", tempDisc.llave);
                strcpy(ruta, tempDisc.direccion);
            }
        }
        fclose(indx);
    }
    return ruta;
}
int getIdParticion(char *idP){
    int i;
    i = 0;
    for (i = 0; i < strlen(idP); i++) {
        if (i == 3) {
            char aux[1];
            aux[1] = '\0';
            aux[0] = idP[3];
            strcpy(idP, aux);
        }
    }
    int id_particion;
    id_particion = atoi(idP);
    printf("<*> ID Particion: %d \n", id_particion);
    return id_particion;
}

void SplitLinea(char* blanco) {
    const char s[2] = "\n";
    char *token;
    token = malloc(sizeof (char));
    token = strtok(blanco, s);
    int contador = 0;
    while (token != NULL) {
        listaU[contador] = malloc(200);
        //printf("%s\n", token);
        strcpy(listaU[contador], token);
        //printf("LISTA POSICION %d: %s\n", contador, listaU[contador]);
        contador = contador + 1;
        token = strtok(NULL, s);
    }
    ncont = contador;
}

void SplitComa(char* linea) {
    const char s[2] = ",";
    char *token;
    token = malloc(200);
    token = strtok(linea, s);
    int contador = 0;
    while (token != NULL) {
        subListaU[contador] = malloc(200);
        strcpy(subListaU[contador], token);
        contador = contador + 1;
        token = strtok(NULL, s);
    }
    mcont = contador;
}

void limpiarListas(){
    int x = 0;
    for(x; x < ncont; x++){
        strcpy(listaU[x], "");
    }
    int z = 0;
    for(z; z < mcont; z++){
        strcpy(subListaU[z], "");
    }
    ncont = 0;
    mcont = 0;
}

void actualizarGrupo(char *aux){
    int x = 0;
    char buffer[64];
    SplitLinea(aux);
    strcpy(buffer, "");
    for(x; x < ncont; x++){
        if(x == auxRmv){
            SplitComa(listaU[x]);
            strcat(buffer, "0,");
            strcat(buffer, subListaU[1]);
            strcat(buffer, ",");
            strcat(buffer, subListaU[2]);
            if((x+1) < ncont)
                strcat(buffer, "\n");
            SplitLinea(aux);
        }else{
            strcat(buffer, listaU[x]);
            if((x+1) < ncont)
                strcat(buffer, "\n");
        }
    }
    strcpy(auxD,buffer);
    printf("\nArchivo: %s", buffer);
}

void loginUser(char *usr, char *pwd, char *idP){
    printf("\n********************** LOGIN *************************\n");
    if(logueado == 0){
        char *rutaA;
        rutaA = malloc(100);
        disco tempDisc;
        MBR auxMBR;
        int inicio;
        inicio = sizeof(MBR);
        char *rutaAux;
        rutaAux = malloc(100);
        strcpy(rutaAux, buscarRutaArchivo(idP));
        int validaLogin = 0;
        /*FILE *indx = fopen("/home/saul/Desktop/manejador.dsk", "rb+");
        if (indx) {
            fseek(indx, 0, SEEK_SET);
            //tempDisc.direccion = malloc(200);

            while (fread(&tempDisc, sizeof (disco), 1, indx)) {*/
                if(validaLogin == 0){
                    //strcpy(rutaA, tempDisc.direccion);
                    strcpy(rutaA, rutaAux);
                    FILE *leer = fopen(rutaA, "rb+");
                    if (leer) {
                        fseek(leer, 0L, SEEK_SET);
                        fread(&auxMBR, sizeof (MBR), 1, leer);
                        int i = 0;
                        printf("<!> DISCO: %s", rutaAux);
                        for (i = 0; i < 4; i++) {
                            if(strcasecmp(auxMBR.mbr_particion[i].part_status, "1") == 0){
                                printf("\n\t* Particion: %s", auxMBR.mbr_particion[i].part_name);
                                i = 4;
                                validaLogin = 1;
                                int byte_inicio = inicio;
                                //printf("\nByte Inicio: %d\n", byte_inicio);
                                SpB auxSB;
                                fseek(leer, byte_inicio, SEEK_SET);
                                fread(&auxSB, sizeof(SpB), 1, leer);
                                //printf("\nAlgo: %d\n", auxSB.sb_arbol_virtual_count);
                                int ibd;
                                ibd = auxSB.sb_ap_bloques;
                                //printf("\n<!> INFO: ibk: %d", ibd);
                                int noEs = auxSB.sb_bloques_count;
                                BD bDatos[noEs];
                                fseek(leer, ibd, SEEK_SET);
                                fread(&bDatos, noEs * sizeof(BD), 1, leer);
                                printf("\n<!> INFO: Datos: %s", bDatos[0].db_data);
                                int tam = strlen(bDatos[0].db_data);
                                printf("\nTamano es: %d\n", tam);
                                SplitLinea(bDatos[0].db_data);
                                int ci = 0;
                                int exito = 0;
                                for(ci = 1; ci < ncont; ci++){
                                    if(listaU[ci] != NULL){
                                        SplitComa(listaU[ci]);
                                        if(strcasecmp(subListaU[1], "U") == 0){
                                            if(strcmp(subListaU[2], usr) == 0){
                                                if(strcmp(subListaU[3], pwd) == 0){
                                                    //verificar las credenciales

                                                    printf("\n<!> INFO: Se a logueado exitosamente.");
                                                    ci == 8;
                                                    exito = 1;
                                                    usuario = malloc(200);
                                                    password = malloc(200);
                                                    strcpy(usuario, usr);
                                                    strcpy(password, pwd);
                                                    logueado = 1;
                                                    limpiarListas();
                                                }
                                            }
                                        }
                                    }
                                }
                                if(exito == 0){
                                    printf("\n<E> ERROR: Usuario o Contrasena invalida.");
                                }
                            }
                            inicio = inicio + auxMBR.mbr_particion[i].part_size;
                        }
                        printf("\n");
                        fclose(leer);
                    }
                }
            /*}
            fclose(indx);
        }*/
    }else{
        printf("\n<E> ERROR: Sesion iniciada existente.");
    }
}

void LogoutUsuario(){
    if(logueado == 0){
        printf("\n<E> ERROR: No hay sesion iniciada.");
    }else{
        strcpy(usuario, "");
        strcpy(password, "");
        logueado = 0;
        printf("\n<!> INFO: Sesion finalizada correctamente.");
    }
}

void mkGrupo(char *idP, char *gname){
    if(logueado == 1){
        if(strcasecmp(usuario, "root") == 0){
            char *ruta;
            int i = 0;
            ruta = malloc(200);
            strcpy(ruta, buscarRutaArchivo(idP));
            int id_particion = getIdParticion(idP);
            FILE *leer = fopen(ruta, "rb+");
            MBR auxMBR;
            int inicio = sizeof(MBR);
            if (leer) { //Archivo existente
                fseek(leer, 0, SEEK_SET);
                fread(&auxMBR, sizeof (MBR), 1, leer);
                for (i = 0; i < 4; i++) {
                    if (auxMBR.mbr_particion[i].part_id == id_particion) {
                        i = 4;
                        int byte_inicio = inicio;
                        SpB auxSB;
                        fseek(leer, byte_inicio, SEEK_SET);
                        fread(&auxSB, sizeof(SpB), 1, leer);
                        //printf("\nAlgo: %d\n", auxSB.sb_arbol_virtual_count);
                        int ibd;
                        ibd = auxSB.sb_ap_bloques;
                        //printf("\n<!> INFO: ibk: %d", ibd);
                        int noEs = auxSB.sb_bloques_count;
                        BD bDatos[noEs];
                        fseek(leer, ibd, SEEK_SET);
                        fread(&bDatos, noEs * sizeof(BD), 1, leer);
                        //printf("\n<!> INFO: Datos: %s", bDatos[0].db_data);

                        char auxData[25];
                        strcpy(auxData, bDatos[0].db_data);
                        SplitLinea(auxData);
                        //printf("\n<!> INFO: Datos0: %s", auxUsers.b_content);
                        int ci = 0;
                        int error = 0;
                        int noG = 0;
                        for(ci = 0; ci < ncont; ci++){
                            if(listaU[ci] != NULL){
                                SplitComa(listaU[ci]);
                                if(strcasecmp(subListaU[1], "G") == 0){
                                    if(strcasecmp(subListaU[2], gname) == 0){
                                        printf("\n<E> ERROR: Grupo existente, no puede crearse.");
                                        //ci == 8;
                                        error = 1;
                                    }
                                    if(atoi(subListaU[0]) != 0)
                                        noG = atoi(subListaU[0]);
                                }
                            }
                        }
                        limpiarListas();
                        if(error == 0){
                            noG = noG + 1;
                            char buffer[14];
                            sprintf(buffer, "%d,G,%s\n", noG, gname);
                            //printf("\nData 0: %s", auxUsers.b_content);
                            strcat(bDatos[0].db_data, buffer);
                            fseek(leer, ibd, SEEK_SET);
                            fwrite(&bDatos, noEs * sizeof(BD), 1, leer);
                            //printf("\nData: %s", auxUsers.b_content);
                            printf("\n<!> INFO: Grupo creado exitosamente.");
                        }
                    }
                    inicio = inicio + auxMBR.mbr_particion[i].part_size;
                }
                fclose(leer);
            }
        }else{
            printf("\n<E> ERROR: No posee permisos de administrador.");
        }
    }else{
        printf("\n<E> ERROR: No hay sesion iniciada.");
    }
}

void rmvGrupo(char *idP, char *gname){
    if(logueado == 1){
        if(strcasecmp(usuario, "root") == 0){
            char *ruta;
            int i = 0;
            ruta = malloc(200);
            strcpy(ruta, buscarRutaArchivo(idP));
            int id_particion = getIdParticion(idP);
            FILE *leer = fopen(ruta, "rb+");
            MBR auxMBR;
            int inicio = sizeof(MBR);
            if (leer) { //Archivo existente
                fseek(leer, 0, SEEK_SET);
                fread(&auxMBR, sizeof (MBR), 1, leer);
                for (i = 0; i < 4; i++) {
                    if (auxMBR.mbr_particion[i].part_id == id_particion) {
                        i = 4;
                        int byte_inicio = inicio;
                        SpB auxSB;
                        fseek(leer, byte_inicio, SEEK_SET);
                        fread(&auxSB, sizeof(SpB), 1, leer);
                        //printf("\nAlgo: %d\n", auxSB.sb_arbol_virtual_count);
                        int ibd;
                        ibd = auxSB.sb_ap_bloques;
                        //printf("\n<!> INFO: ibk: %d", ibd);
                        int noEs = auxSB.sb_bloques_count;
                        BD bDatos[noEs];
                        fseek(leer, ibd, SEEK_SET);
                        fread(&bDatos, noEs * sizeof(BD), 1, leer);

                        char auxData[25];
                        strcpy(auxData, bDatos[0].db_data);
                        SplitLinea(auxData);
                        printf("\n<!> INFO: Datos0: %s", bDatos[0].db_data);
                        int ci = 0;
                        int existe = 0;
                        int noG = 0;
                        for(ci = 0; ci < ncont; ci++){
                            if(listaU[ci] != NULL){
                                //printf("\nLista U1: %s\n", listaU[0]);
                                SplitComa(listaU[ci]);
                                //printf("\nLista U2: %s\n", listaU[0]);
                                if(strcasecmp(subListaU[1], "G") == 0){
                                    if(strcasecmp(subListaU[2], gname) == 0){
                                        strcpy(subListaU[0], "0");
                                        printf("\nGrupo encontrado, preparandose para eliminar...");
                                        auxRmv = ci;
                                        strcpy(auxData, bDatos[0].db_data);
                                        actualizarGrupo(auxData);
                                        //ci == 8;
                                        existe = 1;
                                        //printf("\nResultado:\n%s\n", auxD);
                                        strcpy(bDatos[0].db_data, auxD);
                                        fseek(leer, ibd, SEEK_SET);
                                        fwrite(&bDatos, noEs * sizeof(BD), 1, leer);
                                    }
                                }
                            }
                        }
                        if(existe == 0){
                            printf("\n<E> ERROR: Grupo no encontrado.");
                        }
                        limpiarListas();
                    }
                }
                fclose(leer);
            }
        }else{
            printf("\n<E> ERROR: No posee permisos de administrador.");
        }
    }else{
        printf("\n<E> ERROR: No hay sesion iniciada.");
    }
}


/************************************* USUARIOS **************************************************/

void mkUser(char *idP, char *usr, char *pwd,char *gname){
    if(logueado == 1){
        if(strcasecmp(usuario, "root") == 0){
            char *ruta;
            int i = 0;
            ruta = malloc(200);
            strcpy(ruta, buscarRutaArchivo(idP));
            int id_particion = getIdParticion(idP);
            FILE *leer = fopen(ruta, "rb+");
            MBR auxMBR;
            int inicio = sizeof(MBR);
            if (leer) { //Archivo existente
                fseek(leer, 0, SEEK_SET);
                fread(&auxMBR, sizeof (MBR), 1, leer);
                for (i = 0; i < 4; i++) {
                    if (auxMBR.mbr_particion[i].part_id == id_particion) {
                        i = 4;
                        int byte_inicio = inicio;
                        SpB auxSB;
                        fseek(leer, byte_inicio, SEEK_SET);
                        fread(&auxSB, sizeof(SpB), 1, leer);
                        //printf("\nAlgo: %d\n", auxSB.sb_arbol_virtual_count);
                        int ibd;
                        ibd = auxSB.sb_ap_bloques;
                        //printf("\n<!> INFO: ibk: %d", ibd);
                        int noEs = auxSB.sb_bloques_count;
                        BD bDatos[noEs];
                        fseek(leer, ibd, SEEK_SET);
                        fread(&bDatos, noEs * sizeof(BD), 1, leer);
                        //printf("\n<!> INFO: Datos: %s", bDatos[0].db_data);

                        char auxData[25];
                        strcpy(auxData, bDatos[0].db_data);
                        SplitLinea(auxData);
                        //printf("\n<!> INFO: Datos0: %s", auxUsers.b_content);
                        int ci = 0;
                        int error = 0;
                        int noG = 0;
                        char linea[25];
                        for(ci = 0; ci < ncont; ci++){
                            if(listaU[ci] != NULL){
                                strcpy(linea, listaU[ci]);
                                SplitComa(listaU[ci]);
                                if(strcasecmp(subListaU[1], "U") == 0){
                                    if(strcasecmp(subListaU[3], usr) == 0){
                                        printf("\n<E> ERROR: Usuario existente, no puede crearse.");
                                        //ci == 8;
                                        error = 1;
                                    }
                                    if(strcasecmp(subListaU[2], gname) == 0){
                                        printf("\n<I> INFO: Grupo encontrado.");
                                        //ci == 8;
                                        error = 2;
                                        if(atoi(subListaU[0]) != 0)
                                            noG = atoi(subListaU[0]);
                                    }
                                }
                            }
                        }
                        limpiarListas();
                        if(error == 2){
                            noG = noG + 1;
                            char buffer[14];
                            sprintf(buffer, "%d,U,%s,%s,%s\n", noG, gname, usr, pwd);
                            //printf("\nData 0: %s", auxUsers.b_content);
                            strcat(bDatos[0].db_data, buffer);
                            fseek(leer, ibd, SEEK_SET);
                            fwrite(&bDatos, noEs * sizeof(BD), 1, leer);
                            //printf("\nData: %s", auxUsers.b_content);
                            printf("\n<!> INFO: Usuario creado exitosamente.");
                        }else if(error == 0){
                            printf("\n<E> ERROR: No existe grupo a asignar.\n");
                        }
                    }
                    inicio = inicio + auxMBR.mbr_particion[i].part_size;
                }
                fclose(leer);
            }
        }else{
            printf("\n<E> ERROR: No posee permisos de administrador.");
        }
    }else{
        printf("\n<E> ERROR: No hay sesion iniciada.");
    }
}



void rmvUser(char *idP, char *usr){
    if(logueado == 1){
        if(strcasecmp(usuario, "root") == 0){
            char *ruta;
            int i = 0;
            ruta = malloc(200);
            strcpy(ruta, buscarRutaArchivo(idP));
            int id_particion = getIdParticion(idP);
            FILE *leer = fopen(ruta, "rb+");
            MBR auxMBR;
            int inicio = sizeof(MBR);
            if (leer) { //Archivo existente
                fseek(leer, 0, SEEK_SET);
                fread(&auxMBR, sizeof (MBR), 1, leer);
                for (i = 0; i < 4; i++) {
                    if (auxMBR.mbr_particion[i].part_id == id_particion) {
                        i = 4;
                        int byte_inicio = inicio;
                        SpB auxSB;
                        fseek(leer, byte_inicio, SEEK_SET);
                        fread(&auxSB, sizeof(SpB), 1, leer);
                        //printf("\nAlgo: %d\n", auxSB.sb_arbol_virtual_count);
                        int ibd;
                        ibd = auxSB.sb_ap_bloques;
                        //printf("\n<!> INFO: ibk: %d", ibd);
                        int noEs = auxSB.sb_bloques_count;
                        BD bDatos[noEs];
                        fseek(leer, ibd, SEEK_SET);
                        fread(&bDatos, noEs * sizeof(BD), 1, leer);

                        char auxData[25];
                        strcpy(auxData, bDatos[0].db_data);
                        SplitLinea(auxData);
                        printf("\n<!> INFO: Datos0: %s", bDatos[0].db_data);
                        int ci = 0;
                        int existe = 0;
                        int noG = 0;
                        for(ci = 0; ci < ncont; ci++){
                            if(listaU[ci] != NULL){
                                //printf("\nLista U1: %s\n", listaU[0]);
                                SplitComa(listaU[ci]);
                                //printf("\nLista U2: %s\n", listaU[0]);
                                if(strcasecmp(subListaU[1], "U") == 0){
                                    if(strcasecmp(subListaU[3], usr) == 0){
                                        strcpy(subListaU[0], "0");
                                        printf("\nUsuario encontrado, preparandose para eliminar...");
                                        auxRmv = ci;
                                        strcpy(auxData, bDatos[0].db_data);
                                        actualizarGrupo(auxData);//Parte por cambiar, agregar un nuevo metodo para actualizar el usuario eliminado
                                        //ci == 8;
                                        existe = 1;
                                        //printf("\nResultado:\n%s\n", auxD);
                                        strcpy(bDatos[0].db_data, auxD);
                                        fseek(leer, ibd, SEEK_SET);
                                        fwrite(&bDatos, noEs * sizeof(BD), 1, leer);
                                    }
                                }
                            }
                        }
                        if(existe == 0){
                            printf("\n<E> ERROR: Grupo no encontrado.");
                        }
                        limpiarListas();
                    }
                }
                fclose(leer);
            }
        }else{
            printf("\n<E> ERROR: No posee permisos de administrador.");
        }
    }else{
        printf("\n<E> ERROR: No hay sesion iniciada.");
    }
}



#endif // USERGROUP_H
