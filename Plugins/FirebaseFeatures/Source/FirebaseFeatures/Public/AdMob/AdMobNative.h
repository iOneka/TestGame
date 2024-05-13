// Copyright Pandores Marketplace 2023. All Rights Reserved.

#pragma once

THIRD_PARTY_INCLUDES_START
#	include "firebase/version.h"
THIRD_PARTY_INCLUDES_END

namespace firebase
{
	namespace gma
	{
		class AdView;
		class InterstitialAd;
		namespace rewarded_video {}
		using BannerView = AdView;
		class AdRequest;
	}

	namespace admob
	{
		class BannerView;
		class InterstitialAd;
		namespace rewarded_video {}
		struct AdRequest;
	}
};

#if FIREBASE_VERSION_MAJOR >= 9
using FFirebaseBannerView 		 = firebase::gma::BannerView;
using FFirebaseInterstitial      = firebase::gma::InterstitialAd;;
// Still use old AdMob, API needs to change.
namespace FFirebaseRewardedVideo = firebase::admob::rewarded_video; 
#else
using FFirebaseBannerView 		 = firebase::admob::BannerView;
using FFirebaseInterstitial      = firebase::admob::InterstitialAd;;
namespace FFirebaseRewardedVideo = firebase::admob::rewarded_video;
#endif