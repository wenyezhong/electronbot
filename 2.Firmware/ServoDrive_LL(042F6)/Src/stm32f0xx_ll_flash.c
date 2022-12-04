#include "stm32f0xx_ll_flash.h"
#include "main.h"

void static LL_FLASH_Program_TwoBtye(uint32_t flash_addr,uint16_t data)
{
	LL_FLASH_EnableProgram(FLASH);
	*(__IO uint16_t*)(flash_addr) = data;
}

LL_StatusTypeDef LL_Flash_Unlock(void)
{
	while (LL_FLASH_IsActiveFlag_BSY(FLASH))  
	{
	} 
	if (LL_FLASH_LockState(FLASH)) 
	{ 
		LL_FLASh_SetKey(FLASH,FLASH_KEY1);
		LL_FLASh_SetKey(FLASH,FLASH_KEY2);
	}
	return LL_OK;
}

LL_StatusTypeDef LL_Flash_PageErase(FLASH_EraseInitTypeDef *pEraseInit, uint32_t *PageError)
{
	
	uint32_t address = 0U;

	for(address = pEraseInit->PageAddress;
            address < ((pEraseInit->NbPages * FLASH_PAGE_SIZE) + pEraseInit->PageAddress);
            address += FLASH_PAGE_SIZE)
    {
        LL_FLASH_SetTypeErase(FLASH,FLASH_TYPEERASE_PAGES);
		LL_FLASH_SetEraseADDR(FLASH,address);
		LL_FLASH_StartErase(FLASH); 
		while (LL_FLASH_IsActiveFlag_BSY(FLASH)) 
		{ 
		} 
		if (LL_FLASH_IsActiveFlag_EOP(FLASH)) 
		{ 
			LL_FLASH_ClearFlag_EOP(FLASH);; 
		} 
		else
		{ 
			*PageError = address;
			return LL_ERROR;
		}
		LL_FLASH_DisenableErase(FLASH,FLASH_TYPEERASE_PAGES);
    }
	return LL_OK;
}

LL_StatusTypeDef LL_FLASH_Program(ProgaramDataType ProgramType,uint32_t flash_addr,uint64_t data)
{
	
	uint8_t index = 0U;
	uint8_t nbiterations = 0U;
	
	if(ProgramType == ProgaraType_DATA16)
		nbiterations = 1U;
	else if(ProgramType == ProgaraType_DATA32)
		nbiterations = 2U;
	else
		nbiterations = 4U;
	for(index = 0U; index < nbiterations; index++)
	{
		 LL_FLASH_Program_TwoBtye((flash_addr + (2U*index)), (uint16_t)(data >> (16U*index)));
	}
	
	while (LL_FLASH_IsActiveFlag_BSY(FLASH)) 
	{
	}
	if (LL_FLASH_IsActiveFlag_EOP(FLASH))	
	{
		LL_FLASH_ClearFlag_EOP(FLASH);
	}
	else
	{
		return LL_ERROR;
	}
	LL_FLASH_DisenableProgram(FLASH);
	return LL_OK;
}
