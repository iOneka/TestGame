// Copyright (c) 2021 Nineva Studios

package com.ninevastudios.monetization;

import android.app.Activity;

import androidx.annotation.Keep;
import androidx.annotation.NonNull;
import androidx.annotation.Nullable;

import com.android.billingclient.api.AcknowledgePurchaseParams;
import com.android.billingclient.api.AcknowledgePurchaseResponseListener;
import com.android.billingclient.api.BillingClient;
import com.android.billingclient.api.BillingClientStateListener;
import com.android.billingclient.api.BillingFlowParams;
import com.android.billingclient.api.BillingResult;
import com.android.billingclient.api.ConsumeParams;
import com.android.billingclient.api.ConsumeResponseListener;
import com.android.billingclient.api.PriceChangeConfirmationListener;
import com.android.billingclient.api.PriceChangeFlowParams;
import com.android.billingclient.api.ProductDetails;
import com.android.billingclient.api.ProductDetailsResponseListener;
import com.android.billingclient.api.Purchase;
import com.android.billingclient.api.PurchaseHistoryRecord;
import com.android.billingclient.api.PurchaseHistoryResponseListener;
import com.android.billingclient.api.PurchasesResponseListener;
import com.android.billingclient.api.PurchasesUpdatedListener;
import com.android.billingclient.api.QueryProductDetailsParams;
import com.android.billingclient.api.QueryPurchaseHistoryParams;
import com.android.billingclient.api.QueryPurchasesParams;
import com.android.billingclient.api.SkuDetails;
import com.android.billingclient.api.SkuDetailsParams;
import com.android.billingclient.api.SkuDetailsResponseListener;

import java.util.ArrayList;
import java.util.Arrays;
import java.util.List;

public class MGBilling {

	public static native void OnPurchasesUpdated(BillingResult billingResult, Purchase[] purchases);

	public static native void OnBillingSetupFinished(BillingResult billingResult);

	public static native void OnBillingServiceDisconnected();

	public static native void OnAcknowledgePurchaseResponse(BillingResult billingResult);

	public static native void OnConsumeResponse(BillingResult billingResult, String purchaseToken);

	public static native void OnLaunchBillingFlowStarted(BillingResult billingResult);

	public static native void OnPriceChangeConfirmationResult(BillingResult billingResult);

	public static native void OnPurchaseHistoryResponse(BillingResult billingResult, PurchaseHistoryRecord[] records);

	public static native void OnQueryPurchasesResponse(BillingResult billingResult, Purchase[] purchases);

	public static native void OnSkuDetailsResponse(BillingResult billingResult, SkuDetails[] details);

	public static native void OnProductDetailsResponse(BillingResult billingResult, ProductDetails[] details);

	@Keep
	public static BillingClient createBillingClient(@NonNull Activity activity) {
		return BillingClient.newBuilder(activity)
				.enablePendingPurchases()
				.setListener(new PurchasesUpdatedListener() {
					@Override
					public void onPurchasesUpdated(@NonNull BillingResult billingResult, @Nullable List<Purchase> list) {
						OnPurchasesUpdated(billingResult, list == null ? new Purchase[0] : list.toArray(new Purchase[0]));
					}
				})
				.build();
	}

	@Keep
	public static void startConnection(BillingClient client) {
		client.startConnection(new BillingClientStateListener() {
			@Override
			public void onBillingServiceDisconnected() {
				OnBillingServiceDisconnected();
			}

			@Override
			public void onBillingSetupFinished(@NonNull BillingResult billingResult) {
				OnBillingSetupFinished(billingResult);
			}
		});
	}

	@Keep
	public static void acknowledgePurchase(BillingClient client, String token) {
		AcknowledgePurchaseParams params = AcknowledgePurchaseParams.newBuilder().setPurchaseToken(token).build();
		client.acknowledgePurchase(params, new AcknowledgePurchaseResponseListener() {
			@Override
			public void onAcknowledgePurchaseResponse(@NonNull BillingResult billingResult) {
				OnAcknowledgePurchaseResponse(billingResult);
			}
		});
	}

	@Keep
	public static void consumeItem(BillingClient client, String token) {
		ConsumeParams params = ConsumeParams.newBuilder().setPurchaseToken(token).build();
		client.consumeAsync(params, new ConsumeResponseListener() {
			@Override
			public void onConsumeResponse(@NonNull BillingResult billingResult, @NonNull String purchaseToken) {
				OnConsumeResponse(billingResult, purchaseToken);
			}
		});
	}

	@Keep
	public static void launchBillingFlow(final Activity activity, final BillingClient client, final BillingFlowParams.Builder paramsBuilder) {
		activity.runOnUiThread(new Runnable() {
			@Override
			public void run() {
				BillingResult result = client.launchBillingFlow(activity, paramsBuilder.build());
				OnLaunchBillingFlowStarted(result);
			}
		});
	}

	@Keep
	public static void launchPriceChangeConfirmationFlow(final Activity activity, final BillingClient client, final SkuDetails details) {
		activity.runOnUiThread(new Runnable() {
			@Override
			public void run() {
				PriceChangeFlowParams params = PriceChangeFlowParams.newBuilder().setSkuDetails(details).build();
				client.launchPriceChangeConfirmationFlow(activity, params, new PriceChangeConfirmationListener() {
					@Override
					public void onPriceChangeConfirmationResult(@NonNull BillingResult billingResult) {
						OnPriceChangeConfirmationResult(billingResult);
					}
				});
			}
		});
	}

	@Keep
	public static void queryPurchaseHistoryAsync(BillingClient client, String productType) {
		QueryPurchaseHistoryParams params = QueryPurchaseHistoryParams.newBuilder()
				.setProductType(productType)
				.build();

		client.queryPurchaseHistoryAsync(params, new PurchaseHistoryResponseListener() {
			@Override
			public void onPurchaseHistoryResponse(@NonNull BillingResult billingResult, @Nullable List<PurchaseHistoryRecord> list) {
				OnPurchaseHistoryResponse(billingResult, list == null ? new PurchaseHistoryRecord[0] : list.toArray(new PurchaseHistoryRecord[0]));
			}
		});
	}

	@Keep
	public static void queryPurchasesAsync(@NonNull BillingClient client, @NonNull String productType) {
		client.queryPurchasesAsync(
				QueryPurchasesParams.newBuilder().setProductType(productType).build(),
				new PurchasesResponseListener() {
					@Override
					public void onQueryPurchasesResponse(@NonNull BillingResult billingResult, @Nullable List<Purchase> list) {
						OnQueryPurchasesResponse(billingResult, list == null ? new Purchase[0] : list.toArray(new Purchase[0]));
					}
				});
	}

	@Keep
	public static void querySkuDetailsAsync(BillingClient client, String[] skus, String skuType) {
		SkuDetailsParams params = SkuDetailsParams.newBuilder().setSkusList(Arrays.asList(skus)).setType(skuType).build();
		client.querySkuDetailsAsync(params, new SkuDetailsResponseListener() {
			@Override
			public void onSkuDetailsResponse(@NonNull BillingResult billingResult, @Nullable List<SkuDetails> list) {
				OnSkuDetailsResponse(billingResult, list == null ? new SkuDetails[0] : list.toArray(new SkuDetails[0]));
			}
		});
	}

	@Keep
	public static void queryProductDetailsAsync(@NonNull BillingClient client, @NonNull String[] productIds, @NonNull String productType) {
		List<QueryProductDetailsParams.Product> products = new ArrayList<>();
		for (String productId : productIds) {
			products.add(QueryProductDetailsParams.Product.newBuilder()
					.setProductId(productId)
					.setProductType(productType)
					.build());
		}

		QueryProductDetailsParams params = QueryProductDetailsParams.newBuilder()
				.setProductList(products)
				.build();
		client.queryProductDetailsAsync(params, new ProductDetailsResponseListener() {
			@Override
			public void onProductDetailsResponse(@NonNull BillingResult billingResult, @NonNull List<ProductDetails> list) {
				OnProductDetailsResponse(billingResult, list.toArray(new ProductDetails[0]));
			}
		});
	}

	@Keep
	public static BillingFlowParams.Builder createBillingFlowParamsBuilder() {
		return BillingFlowParams.newBuilder();
	}

	@Keep
	public static void setSubscriptionUpdateParams(BillingFlowParams.Builder builder, String oldSkuPurchaseToken, int prorationMode) {
		BillingFlowParams.SubscriptionUpdateParams.Builder updateParamsBuilder = BillingFlowParams.SubscriptionUpdateParams.newBuilder();
		if (prorationMode != BillingFlowParams.ProrationMode.IMMEDIATE_WITH_TIME_PRORATION) {
			updateParamsBuilder.setReplaceProrationMode(prorationMode);
		}
		if (!oldSkuPurchaseToken.isEmpty()) {
			updateParamsBuilder.setOldPurchaseToken(oldSkuPurchaseToken);
		}
		builder.setSubscriptionUpdateParams(updateParamsBuilder.build());
	}

	@Keep
	public static void setProductDetailsParams(BillingFlowParams.Builder builder, ProductDetails productDetails, String offerToken) {
		BillingFlowParams.ProductDetailsParams.Builder paramsBuilder = BillingFlowParams.ProductDetailsParams.newBuilder();
		paramsBuilder.setProductDetails(productDetails);

		if (!offerToken.isEmpty()) {
			paramsBuilder.setOfferToken(offerToken);
		}

		List<BillingFlowParams.ProductDetailsParams> productDetailsParamsList = new ArrayList<>();
		productDetailsParamsList.add(paramsBuilder.build());

		builder.setProductDetailsParamsList(productDetailsParamsList);
	}

	@Keep
	public static String[] getPurchaseSkus(Purchase purchase) {
		return purchase.getProducts().toArray(new String[0]);
	}

	@Keep
	public static String[] getPurchaseSkusForRecord(PurchaseHistoryRecord purchaseHistoryRecord) {
		return purchaseHistoryRecord.getProducts().toArray(new String[0]);
	}

	@Keep
	public static ProductDetails.SubscriptionOfferDetails[] getSubscriptionOfferDetails(ProductDetails productDetails) {
		List<ProductDetails.SubscriptionOfferDetails> details = productDetails.getSubscriptionOfferDetails();
		if (details != null) {
			return details.toArray(new ProductDetails.SubscriptionOfferDetails[0]);
		}

		return new ProductDetails.SubscriptionOfferDetails[0];
	}

	@Keep
	public static ProductDetails.PricingPhase[] getPricingPhases(ProductDetails.SubscriptionOfferDetails subscriptionOfferDetails) {
		List<ProductDetails.PricingPhase> details = subscriptionOfferDetails.getPricingPhases().getPricingPhaseList();
		return details.toArray(new ProductDetails.PricingPhase[0]);
	}

	@Keep
	public static String[] getOfferTags(ProductDetails.SubscriptionOfferDetails subscriptionOfferDetails) {
		return subscriptionOfferDetails.getOfferTags().toArray(new String[0]);
	}
}
