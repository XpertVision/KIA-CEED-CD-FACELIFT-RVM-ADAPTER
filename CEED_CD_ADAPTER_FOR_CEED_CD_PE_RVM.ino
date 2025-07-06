#include <SPI.h>
#include <mcp2515.h>

//CAN Data pin
MCP2515 mcp2515(10);

//CAN Frame variables
can_frame canMsg;
can_frame canTVM_CD_PE;
can_frame canNormal_CD_PE;

//CAN Message IDs
uint32_t idRVM          = 0x4A4;
uint32_t idShiftPostion = 0x4A9;
uint32_t idStartEngine  = 0x553;
uint32_t idACCButton    = 0x541;

//CAN data statments
uint8_t reverseAndBreakeOn7Data          = 0x60;
uint8_t reverseOn7Data                   = 0x20;
uint8_t cameraViewMode0Data              = 0x4;
uint8_t startEngineOnWrongShift7Data     = 0x1C;
uint8_t accTryToStartEngineProccess0Data = 0x04;

//Code logic statments
bool bIsTVM               = false;
bool bIsNeedResetParams   = false;
bool bIsWasStartEngineTry = false;
uint8_t iCounter          = 0;

//Function for encapsulated RVM adaptation logic
void RVMAdaptation(can_frame& canFrame);

void setup()
{
  canTVM_CD_PE.can_id  = 0x4A4;
  canTVM_CD_PE.can_dlc = 8;
  canTVM_CD_PE.data[0] = 0x3F;
  canTVM_CD_PE.data[1] = 0x02;
  canTVM_CD_PE.data[2] = 0x00;
  canTVM_CD_PE.data[3] = 0x00;
  canTVM_CD_PE.data[4] = 0x00;
  canTVM_CD_PE.data[5] = 0x00;
  canTVM_CD_PE.data[6] = 0x00;
  canTVM_CD_PE.data[7] = 0x00;

  canNormal_CD_PE.can_id  = 0x4A4;
  canNormal_CD_PE.can_dlc = 8;
  canNormal_CD_PE.data[0] = 0x3F;
  canNormal_CD_PE.data[1] = 0x01;
  canNormal_CD_PE.data[2] = 0x00;
  canNormal_CD_PE.data[3] = 0x00;
  canNormal_CD_PE.data[4] = 0x00;
  canNormal_CD_PE.data[5] = 0x00;
  canNormal_CD_PE.data[6] = 0x00;
  canNormal_CD_PE.data[7] = 0x00;

  mcp2515.reset();
  mcp2515.setBitrate(CAN_500KBPS, MCP_8MHZ);
  mcp2515.setNormalMode();
}

void loop()
{
  if (mcp2515.readMessage(&canMsg) == MCP2515::ERROR_OK)
    RVMAdaptation(canMsg);
}

void RVMAdaptation(can_frame& canFrame)
{
  //Logic for camera mods when gears selects
  if(canFrame.can_id == idShiftPostion)
  {
    if(canFrame.can_dlc == 8 && (canFrame.data[7] == reverseOn7Data || canFrame.data[7] == reverseAndBreakeOn7Data) && bIsNeedResetParams)
    {
      bIsTVM             = false;
      iCounter           = 0;
      bIsNeedResetParams = false;

      return;
    }

    if(canFrame.can_dlc == 8 && canFrame.data[7] != reverseAndBreakeOn7Data && canFrame.data[7] != reverseOn7Data)
    {
      bIsNeedResetParams = true;

      return;
    }
  }
  //Logic for camera when try to start engine on wrong gear
  else if(canFrame.can_id == idStartEngine && canFrame.can_dlc == 8)
  {
    if(canFrame.data[7] == startEngineOnWrongShift7Data && bIsWasStartEngineTry)
    {
      bIsTVM               = false;
      iCounter             = 0;
      bIsWasStartEngineTry = false;
    }
      
    return;
  }
  //Detect try for start engine (no matter success or not)
  else if(canFrame.can_id == idACCButton && canFrame.can_dlc == 8)
  {
    if(canFrame.data[0] == accTryToStartEngineProccess0Data)
      bIsWasStartEngineTry = true;

    return;
  }

  //Return if CAN message not about rear view camera
  if(canFrame.can_id != idRVM)
    return;

  //Logic for rear view camera mode selecting
  if(canFrame.data[0] == cameraViewMode0Data)
  {
    ++iCounter;

    if(bIsTVM == false)
    {
      mcp2515.sendMessage(&canTVM_CD_PE);
      if(iCounter == 3)
      {
        bIsTVM   = !bIsTVM;
        iCounter = 0;
      }
    }
    else
    {
      mcp2515.sendMessage(&canNormal_CD_PE);
      if(iCounter == 3)
      {
        bIsTVM   = !bIsTVM;
        iCounter = 0;
      }
    }
  }
}