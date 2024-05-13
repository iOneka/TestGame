// Copyright (c) 2021 Nineva Studios

#pragma once

#import <StoreKit/StoreKit.h>

@interface MGProductsRequestDelegate : NSObject <SKProductsRequestDelegate>

@property (nonatomic, copy) void (^onResponseReceived)(SKProductsResponse* response);

@property (nonatomic, copy) void (^onError)(NSError* error);

@end
