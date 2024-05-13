// Copyright (c) 2021 Nineva Studios

#pragma once

#import <StoreKit/StoreKit.h>

@interface MGPaymentQueueDelegate : NSObject <SKPaymentQueueDelegate>

@property (nonatomic, copy) bool (^shouldContinueTransactionDelegate)(SKPaymentTransaction* transaction, SKStorefront* newStorefront) API_AVAILABLE(ios(13)) ;

@property (nonatomic, copy) bool (^shouldShowPriceConsentDelegate)();

@end
