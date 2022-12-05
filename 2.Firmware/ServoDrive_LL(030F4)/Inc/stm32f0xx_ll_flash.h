#ifndef __STM32F0xx_LL_FLASH_EX_H
#define __STM32F0xx_LL_FLASH_EX_H

#include "stm32f0xx.h"
#include "stm32f0xx_ll_flash.h"
typedef enum {
	LL_OK=0U,
	LL_ERROR=1U,
	LL_BUSY = 2U,
	LL_TIMEOUT=3U
}LL_StatusTypeDef;


typedef struct
{
  uint32_t TypeErase;   /*!< TypeErase: Mass erase or page erase.
                             This parameter can be a value of @ref FLASHEx_Type_Erase */

  uint32_t PageAddress; /*!< PageAdress: Initial FLASH page address to erase when mass erase is disabled
                             This parameter must be a number between Min_Data = FLASH_BASE and Max_Data = FLASH_BANK1_END */
  
  uint32_t NbPages;     /*!< NbPages: Number of pagess to be erased.
                             This parameter must be a value between Min_Data = 1 and Max_Data = (max number of pages - value of initial page)*/
                                                          
} FLASH_EraseInitTypeDef;

#define FLASH_FLAG_BSY             FLASH_SR_BSY            /*!< FLASH Busy flag                           */ 
#define FLASH_FLAG_PGERR           FLASH_SR_PGERR          /*!< FLASH Programming error flag    */
#define FLASH_FLAG_WRPERR          FLASH_SR_WRPERR         /*!< FLASH Write protected error flag          */
#define FLASH_FLAG_EOP             FLASH_SR_EOP            /*!< FLASH End of Operation flag               */

#define FLASH_TYPEERASE_PAGES          FLASH_CR_PER         /*!< FLASH_CR_PER          */
#define FLASH_TYPEERASE_MASSERASE      FLASH_CR_MER            /*!< MASSERASE              */


#if defined(STM32F030x6) || defined(STM32F030x8) || defined(STM32F031x6) || defined(STM32F038xx) \
 || defined(STM32F051x8) || defined(STM32F042x6) || defined(STM32F048xx) || defined(STM32F058xx) || defined(STM32F070x6)
#define FLASH_PAGE_SIZE          0x400U
#endif /* STM32F030x6 || STM32F030x8 || STM32F031x6 || STM32F051x8 || STM32F042x6 || STM32F048xx || STM32F058xx || STM32F070x6 */

#if defined(STM32F071xB) || defined(STM32F072xB) || defined(STM32F078xx) || defined(STM32F070xB) \
 || defined(STM32F091xC) || defined(STM32F098xx) || defined(STM32F030xC)
#define FLASH_PAGE_SIZE          0x800U
#endif /* STM32F071xB || STM32F072xB || STM32F078xx || STM32F091xC || STM32F098xx || STM32F030xC */

typedef enum {
	ProgaraType_DATA64,
	ProgaraType_DATA32,
	ProgaraType_DATA16
}ProgaramDataType;

typedef enum {\
	FLASH_Lock=1U,Flash_Unlock=!FLASH_Lock\
}FlashStates;

  /* Set the OBL_Launch bit to launch the option byte loading */
__STATIC_INLINE void LL_FLASH_SET_OBL_Launch(FLASH_TypeDef *FLASHx)
{
  SET_BIT(FLASHx->CR, FLASH_CR_OBL_LAUNCH);
}
__STATIC_INLINE void LL_FLASH_Lock(void)
{
  SET_BIT(FLASH->CR, FLASH_CR_LOCK);
}


  /* @brief  Set flash erase type.
  * @param  FLASH_TYPEERASE specifies the FLASH flags to clear.
  *          This parameter can be any combination of the following values:
  *            @arg @ref FLASH_TYPEERASE_PAGES         PAGES Erase
  *            @arg @ref FLASH_TYPEERASE_MASSERASE      FLASH Write protected error flag 
  * @retval none*/

__STATIC_INLINE void LL_FLASH_SetTypeErase(FLASH_TypeDef *FLASHx,uint32_t FLASH_TYPEERASE)
{
  SET_BIT(FLASHx->CR, FLASH_TYPEERASE);
}
  /* @brief  Set flash erase ADDR.
  *          This parameter can be any combination of the following values:
  *            @arg @ref EraseADDR         uint32_t value
  * @retval none*/

__STATIC_INLINE void LL_FLASH_SetEraseADDR(FLASH_TypeDef *FLASHx,uint32_t EraseADDR)
{
  WRITE_REG(FLASHx->AR, EraseADDR);
}
  /* @brief  Set flash erase ADDR.
  *          This parameter can be any combination of the following values:
  *            @arg @ref EraseADDR         uint32_t value
  * @retval none*/

__STATIC_INLINE void LL_FLASH_StartErase(FLASH_TypeDef *FLASHx)
{
  SET_BIT(FLASHx->CR, FLASH_CR_STRT);
}

  /* @brief  Clear the specified FLASH flag.
  * @param  __FLAG__ specifies the FLASH flags to clear.
  *          This parameter can be any combination of the following values:
  *            @arg @ref FLASH_FLAG_EOP         FLASH End of Operation flag 
  *            @arg @ref FLASH_FLAG_WRPERR      FLASH Write protected error flag 
  *            @arg @ref FLASH_FLAG_PGERR       FLASH Programming error flag
  * @retval none*/

__STATIC_INLINE void LL_FLASH_ClearFlag(uint32_t STATE_FLAG)
{
  WRITE_REG(FLASH->SR, STATE_FLAG);
}

  /*get bit flash bsy*/
__STATIC_INLINE uint32_t LL_FLASH_IsActiveFlag_BSY(FLASH_TypeDef *FLASHx)
{
  return (READ_BIT(FLASHx->SR, FLASH_SR_BSY) == (FLASH_SR_BSY));
}
/*get end of operation bilt*/
__STATIC_INLINE uint32_t LL_FLASH_IsActiveFlag_EOP(FLASH_TypeDef *FLASHx)
{
  return (READ_BIT(FLASHx->SR, FLASH_SR_EOP) == (FLASH_SR_EOP));
}
/*clear end of operation bilt*/
__STATIC_INLINE void LL_FLASH_ClearFlag_EOP(FLASH_TypeDef *FLASHx)
{
  SET_BIT(FLASHx->SR, FLASH_SR_EOP);//EOP bit Set clear
}
  /* @brief  Set flash erase type.
  * @param  FLASH_TYPEERASE specifies the FLASH flags to clear.
  *          This parameter can be any combination of the following values:
  *            @arg @ref FLASH_TYPEERASE_PAGES         PAGES Erase
  *            @arg @ref FLASH_TYPEERASE_MASSERASE      FLASH Write protected error flag 
  * @retval none*/
__STATIC_INLINE void LL_FLASH_DisenableErase(FLASH_TypeDef *FLASHx,uint32_t FLASH_TYPEERASE)
{
  CLEAR_BIT(FLASHx->CR, FLASH_TYPEERASE);
}

/*EnableProgram*/
__STATIC_INLINE void LL_FLASH_EnableProgram(FLASH_TypeDef *FLASHx)
{
  SET_BIT(FLASHx->CR,FLASH_CR_PG);
}
/*DisenableProgram*/
__STATIC_INLINE void LL_FLASH_DisenableProgram(FLASH_TypeDef *FLASHx)
{
  CLEAR_BIT(FLASHx->CR,FLASH_CR_PG);
}
/*read flash's states of lock or unlock*/
__STATIC_INLINE FlashStates LL_FLASH_LockState(FLASH_TypeDef *FLASHx)
{
	return (FlashStates)(READ_BIT(FLASHx->CR,FLASH_CR_LOCK));
}
/*set key for flash*/
__STATIC_INLINE void LL_FLASh_SetKey(FLASH_TypeDef *FLASHx,uint32_t key)
{
	WRITE_REG(FLASH->KEYR,key);
}


LL_StatusTypeDef LL_Flash_Unlock(void);
LL_StatusTypeDef LL_Flash_PageErase(FLASH_EraseInitTypeDef *pEraseInit, uint32_t *PageError);
LL_StatusTypeDef LL_FLASH_Program(ProgaramDataType ProgramType,uint32_t flash_addr,uint64_t data);
#endif
