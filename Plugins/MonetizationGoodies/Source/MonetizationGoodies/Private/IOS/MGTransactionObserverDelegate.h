// Copyright (c) 2021 Nineva Studios

#pragma once

#import <StoreKit/StoreKit.h>

@interface MGTransactionObserverDelegate : NSObject <SKPaymentTransactionObserver>

@property (nonatomic, copy) void (^onTransactionsUpdated)(NSArray<SKPaymentTransaction*>* transactions);

@property (nonatomic, copy) void (^onTransactionsRemoved)(NSArray<SKPaymentTransaction*>* transactions);

@property (nonatomic, copy) void (^onRestoreTransactionsCompleted)();

@property (nonatomic, copy) void (^onRestoreTransactionsError)(NSError* error);

@property (nonatomic, copy) void (^onDownloadsUpdated)(NSArray<SKDownload*>* downloads);

@property (nonatomic, copy) bool (^shouldAddStorePaymentForProduct)(SKPayment* payment, SKProduct *product);

@property (nonatomic, copy) void (^onEntitlementsRevoked)(NSArray<NSString*>* entitlements);

@property (nonatomic, copy) void (^onStoreFrontChanged)();

@end
