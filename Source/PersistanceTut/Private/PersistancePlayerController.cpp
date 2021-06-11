// Fill out your copyright notice in the Description page of Project Settings.


#include "PersistancePlayerController.h"
#include "PersistanceTut/PersistanceTutCharacter.h"
#include "PersistanceTut/PersistanceTutGameMode.h"

#include "JsonObjectConverter.h"

APersistancePlayerController::APersistancePlayerController()
{
	Http = &FHttpModule::Get();
}

void APersistancePlayerController::BeginPlay()
{
	if (!HasAuthority())
	{
		UE_LOG(LogTemp, Warning, TEXT("RUNNING ON CLIENT"));
	}
	else
	{
		UE_LOG(LogTemp, Warning, TEXT("RUNNING ON SERVER"));
		FTimerHandle TSaveHandle;
		GetWorldTimerManager().SetTimer(TSaveHandle, this, &APersistancePlayerController::SaveData, 5.0f, true);
	}
}

void APersistancePlayerController::HandleServerEntry()
{
	if (!HasAuthority())
	{
		return;
	}

	FString PID = "1235";
	
	TSharedRef<IHttpRequest, ESPMode::ThreadSafe> Request = Http->CreateRequest();

	Request->OnProcessRequestComplete().BindUObject(this, &APersistancePlayerController::OnProcessRequestComplete);
	Request->SetURL("http://localhost:8080/api/PlayerData/" + PID);
	Request->SetVerb("GET");
	Request->SetHeader(TEXT("Content-Type"), TEXT("application/json"));

	// Get Request through API passing in PID
	Request->ProcessRequest();
}

void APersistancePlayerController::OnProcessRequestComplete(FHttpRequestPtr Request, FHttpResponsePtr Response,	bool Success)
{
	FVector Location = FVector::ZeroVector;
	Location.Z = 400.0f;
	FPlayerData PlayerData;
	
	if (Success)
	{
		// setup pawn
		UE_LOG(LogTemp, Warning, TEXT("SUCCESS %s"), *Response->GetContentAsString());

		PlayerData = ConvertToPlayerData(Response->GetContentAsString());
		if (PlayerData.isvalid)
		{
			UE_LOG(LogTemp, Warning, TEXT("SUCCESS %f"), PlayerData.Zcoord);
			Location.X = PlayerData.Xcoord;
			Location.Y = PlayerData.Ycoord;
			Location.Z = PlayerData.Zcoord;
		}
	}
	else
	{
		UE_LOG(LogTemp, Warning, TEXT("FAILED"));
	}

	if (APersistanceTutGameMode* GM = GetWorld()->GetAuthGameMode<APersistanceTutGameMode>())
	{
		FActorSpawnParameters SpawnParams;
		SpawnParams.Owner = this;
		SpawnParams.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AlwaysSpawn;
			
		if (APersistanceTutCharacter* NewPawn = GetWorld()->SpawnActor<APersistanceTutCharacter>(GM->DefaultPawnClass, Location, FRotator::ZeroRotator, SpawnParams))
		{
			NewPawn->SetHealth(PlayerData.Health);
			Possess(NewPawn);
		}
	}
}

FPlayerData APersistancePlayerController::ConvertToPlayerData(const FString& ResponseString)
{
	FPlayerData PlayerData;
	if (!ResponseString.Contains("timestamp"))
	{
		FJsonObjectConverter::JsonObjectStringToUStruct(*ResponseString, &PlayerData, 0, 0);
	}

	return PlayerData;
}

void APersistancePlayerController::SaveData()
{
	UE_LOG(LogTemp, Warning, TEXT("SAVING"));
	APersistanceTutCharacter* ControlledCharacter = GetPawn<APersistanceTutCharacter>();
	if (ControlledCharacter)
	{
		FVector Location = ControlledCharacter->GetActorLocation();
		FPlayerData PlayerData;
		PlayerData.isvalid = true;
		PlayerData.pid = 1235;
		PlayerData.Health = ControlledCharacter->GetHealth();
		PlayerData.Xcoord = Location.X;
		PlayerData.Ycoord = Location.Y;
		PlayerData.Zcoord = Location.Z;

		TSharedRef<IHttpRequest, ESPMode::ThreadSafe> Request = Http->CreateRequest();

		Request->SetURL("http://localhost:8080/api/PlayerData/");
		Request->SetVerb("PUT");
		Request->SetHeader(TEXT("Content-Type"), TEXT("application/json"));

		FString JsonString;
		FJsonObjectConverter::UStructToJsonObjectString(PlayerData, JsonString);
		Request->SetContentAsString(JsonString);

		// Post Request through API passing in PID
		Request->ProcessRequest();
	}
}
