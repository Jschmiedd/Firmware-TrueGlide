/*
 * TrueGlide - carte simulée : convertisseur analogique (ADC).
 * Free software under the GPL-3.0 license, WITHOUT ANY WARRANTY; see the LICENSE file.
 *
 * Les mesures internes (tension de référence, température de la puce) sont terminées dès qu'elles sont lancées ;
 * leurs valeurs sont fixées par tgMemoireInit(). Les mesures externes (batterie, courant, RSSI) arrivent normalement
 * par DMA : aucune batterie n'est branchée sur la carte simulée, elles restent à zéro.
 */

#include "platform.h"

void tgMaterielAdcStartInjected(ADC_TypeDef *ADCx);

void ADC_SoftwareStartInjectedConv(ADC_TypeDef *ADCx)
{
    tgMaterielAdcStartInjected(ADCx);
    ADCx->SR |= ADC_FLAG_JEOC;
}
