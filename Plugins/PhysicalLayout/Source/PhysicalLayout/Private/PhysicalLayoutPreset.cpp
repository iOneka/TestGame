// Fill out your copyright notice in the Description page of Project Settings.
#include "PhysicalLayoutPreset.h"


UPhysicalLayoutPreset_Factory::UPhysicalLayoutPreset_Factory(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{
	SupportedClass = UPhysicalLayoutPreset::StaticClass();

	bCreateNew = true;
}


